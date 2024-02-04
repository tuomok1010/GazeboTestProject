#include "PacketHandler.h"

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <netdb.h>
#include <netinet/ip.h>
#include <arpa/inet.h>

// TODO: add option to supply a specific IP address to bind on
int PHInit(struct InitData* initData, const char* port)
{
    memset(initData->pollFds, -1, sizeof(initData->pollFds));
    memset(initData->packetBuffer, 0, PACKET_BUFFER_LEN);

    struct addrinfo* results = NULL;
    struct addrinfo* result = NULL;
    struct addrinfo hints;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    /*
     * fills the addrinfo array with valid addrinfo structs
     * NOTE: passing NULL as the nodeName and using AI_PASSIVE flag in hints allows us to set up
     * a socket that listens on all IP addresses of this machine for incoming connections to the port
    */
    int res = getaddrinfo(NULL, port, &hints, &results);
    if(res != 0)
    {
        fprintf(stderr, "getaddrinfo() error\n");
        return -1;
    }

    for(result = results; result != NULL; result = result->ai_next)
    {
        initData->pollFds[LISTENER_POLLFD_INDEX].fd = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
        if(initData->pollFds[LISTENER_POLLFD_INDEX].fd == -1)
        {
            fprintf(stderr, "socket() error, retrying..\n");
            continue;
        }

        /*
         * allows us to bind() to an address+port already in use
         * if we force quit the app, the addr+port will be in use for a while
         * and without this option, we will not be able to bind() to them again
         * (this may not be necessary since we do not bind to a specific ip address, TODO: find out)
        */
        int optVal = 1;
        if(setsockopt(initData->pollFds[LISTENER_POLLFD_INDEX].fd, SOL_SOCKET, SO_REUSEADDR, (char*)&optVal, sizeof(int)) == -1)
        {
            fprintf(stderr, "setsockopt() error, retrying..\n");
            close(initData->pollFds[LISTENER_POLLFD_INDEX].fd);
            continue;
        }

        // binds a socket to an IP address (in our case any ip address) and port
        if(bind(initData->pollFds[LISTENER_POLLFD_INDEX].fd, result->ai_addr, result->ai_addrlen) == -1)
        {
            fprintf(stderr, "bind() error, retrying..\n");
            close(initData->pollFds[LISTENER_POLLFD_INDEX].fd);
            continue;
        }

        // set up socket for listening for incoming connections NOTE: this does NOT block
        if(listen(initData->pollFds[LISTENER_POLLFD_INDEX].fd, SOMAXCONN) == -1)
        {
            fprintf(stderr, "listen() error, retrying..\n");
            close(initData->pollFds[LISTENER_POLLFD_INDEX].fd);
            continue;
        }

        char listenerAddrStr[INET6_ADDRSTRLEN];
        memset(listenerAddrStr, 0, sizeof(listenerAddrStr));
        if(PHAddrToStr(result->ai_addr, listenerAddrStr, sizeof(listenerAddrStr)) != 0)
        {
            fprintf(stderr, "PHAddrToStr() error\n");
            close(initData->pollFds[LISTENER_POLLFD_INDEX].fd);
            continue;
        }

        fprintf(stdout, "%s listening on port %s\n", listenerAddrStr, port);
        break;
    }

    // we looped through all available addrinfo structs, but none was a success
    if(result == NULL)
    {
        fprintf(stderr, "unable to create socket, exiting..\n");
        shutdown(initData->pollFds[LISTENER_POLLFD_INDEX].fd, SHUT_RDWR);
        close(initData->pollFds[LISTENER_POLLFD_INDEX].fd);
        freeaddrinfo(results);
        return -1;
    }

    // we no longer need the addrInfo structs
    freeaddrinfo(results);

    initData->pollFds[LISTENER_POLLFD_INDEX].events = POLLIN;
    initData->pollFds[LISTENER_POLLFD_INDEX].revents = 0;

    return 0;
}

int PHAcceptConnection(int32_t listeningSocket)
{
    struct sockaddr_storage clientAddr;
    socklen_t clientAddrLen = sizeof(clientAddr);
    memset(&clientAddr, 0, sizeof(clientAddr));

    // accept incoming connection and fill clientAddr with client's info
    // TODO: can we filter incoming connections?
    int newFd = accept(listeningSocket, (struct sockaddr*)&clientAddr, &clientAddrLen);
    if(newFd == -1)
    {
        fprintf(stderr, "accept() error");
        return -1;
    }

    char clientAddrStr[INET6_ADDRSTRLEN];
    memset(clientAddrStr, 0, sizeof(clientAddrStr));
    if(PHAddrToStr((struct sockaddr*)&clientAddr, clientAddrStr, sizeof(clientAddrStr)) != 0)
    {
        fprintf(stderr, "PHAddrToStr() error\n");
        shutdown(newFd, SHUT_RDWR);
        close(newFd);
        return -2;
    }

    printf("client %s connected\n", clientAddrStr);
    return newFd;
}

int PHReceive(int32_t socketFd, char* packetBuffer, uint32_t packetBufferSizeInBytes, PacketReceivedCallback cb)
{
    int32_t nBytes = recv(socketFd, packetBuffer, packetBufferSizeInBytes - 1, 0);
    if(nBytes == 0)
    {
        // handle disconnect
        fprintf(stderr, "client connection closed");
        return -1;
    }
    if(nBytes < 0)
    {
        fprintf(stderr, "recvfrom() error\n");
        return -2;
    }

    struct sockaddr_storage clientAddr;
    socklen_t clientAddrLen = sizeof(clientAddr);
    memset(&clientAddr, 0, sizeof(clientAddr));
    if(getpeername(socketFd, (struct sockaddr*)&clientAddr, &clientAddrLen) < 0)
    {
        fprintf(stderr, "getpeername() error\n");
        return -3;
    }

    struct ClientInfo clientInfo;
    memset(&clientInfo, 0, sizeof(clientInfo));
    if(PHAddrAndPortToStr((struct sockaddr*)&clientAddr, clientInfo.addrStr, sizeof(clientInfo.addrStr), clientInfo.portStr, sizeof(clientInfo.portStr)) != 0)
    {
        fprintf(stderr, "PHAddrAndPortToStr() error\n");
        return -4;
    }

    cb(packetBuffer, nBytes, &clientInfo);
    return 0;
}

int PHReceiveLoop(struct pollfd* pollFds, uint32_t nPollFds, int32_t pollTimeOut, 
    char* packetBuffer, uint32_t packetBufferSizeInBytes, PacketReceivedCallback cb)
{
    for(;;)
    {
        int nEvents = poll(pollFds, nPollFds, pollTimeOut);
        if(nEvents == -1)
        {
            fprintf(stderr, "poll() error\n");
            PHRemoveAndCloseAllFromPollArray(pollFds, nPollFds);
            return -1;
        }

        for(unsigned int i = 0; i < nPollFds; ++i)
        {
            if(pollFds[i].revents & POLLIN)
            {
                if(i == LISTENER_POLLFD_INDEX)
                {
                    int newFd = PHAcceptConnection(pollFds[i].fd);
                    // TODO: can a valid socket be 0??
                    if(newFd > 0)
                    {
                        if(PHAddToPollArray(pollFds, nPollFds, newFd) != 0)
                        {
                            fprintf(stderr, "AddToPollArray()error \n"); 
                            shutdown(newFd, SHUT_RDWR);
                            close(newFd);
                        }
                    }
                }
                else
                {
                    // handle incoming data
                    int res = PHReceive(pollFds[i].fd, packetBuffer, packetBufferSizeInBytes, cb);
                    if(res < 0)
                    {
                        PHRemoveAndCloseFromPollArray(pollFds, nPollFds, pollFds[i].fd);
                    }
                }
            }
        }
    }
}

int PHReceiveNoLoop(struct pollfd* pollFds, uint32_t nPollFds, int32_t pollTimeOut, 
    char* packetBuffer, uint32_t packetBufferSizeInBytes, PacketReceivedCallback cb)
{
    int nEvents = poll(pollFds, nPollFds, pollTimeOut);
    if(nEvents == -1)
    {
        fprintf(stderr, "poll() error\n");
        PHRemoveAndCloseAllFromPollArray(pollFds, nPollFds);
        return -1;
    }

    for(unsigned int i = 0; i < nPollFds; ++i)
    {
        if(pollFds[i].revents & POLLIN)
        {
            if(i == LISTENER_POLLFD_INDEX)
            {
                int newFd = PHAcceptConnection(pollFds[i].fd);
                // TODO: can a valid socket be 0??
                if(newFd > 0)
                {
                    if(PHAddToPollArray(pollFds, nPollFds, newFd) != 0)
                    {
                        fprintf(stderr, "AddToPollArray()error \n"); 
                        shutdown(newFd, SHUT_RDWR);
                        close(newFd);
                    }
                }
            }
            else
            {
                // handle incoming data
                int res = PHReceive(pollFds[i].fd, packetBuffer, packetBufferSizeInBytes, cb);
                if(res < 0)
                {
                    PHRemoveAndCloseFromPollArray(pollFds, nPollFds, pollFds[i].fd);
                }
            }
        }
    }

    return 0;
}

int PHAddToPollArray(struct pollfd* pollFds, uint32_t nPollFds, int32_t socketToAdd)
{
    for(unsigned int i = 0; i < nPollFds; ++i)
    {
        // -1 means a free socket
        if(pollFds[i].fd == -1)
        {
            pollFds[i].fd = socketToAdd;
            pollFds[i].events = POLLIN;
            pollFds[i].revents = 0;
            return 0;
        }
    }

    return -1;
}

int PHRemoveAndCloseFromPollArray(struct pollfd* pollFds, uint32_t nPollFds, int32_t socketToRemove)
{
    for(unsigned int i = 0; i < nPollFds; ++i)
    {
        if(pollFds[i].fd == socketToRemove)
        {
            shutdown(pollFds[i].fd, SHUT_RDWR);
            close(pollFds[i].fd);
            memset(&pollFds[i], -1, sizeof(pollFds[i]));
            return 0;
        }
    }

    return -1;
}

void PHRemoveAndCloseAllFromPollArray(struct pollfd* pollFds, uint32_t nPollFds)
{
    for(uint32_t i = 0; i < nPollFds; ++i)
    {
        shutdown(pollFds[i].fd, SHUT_RDWR);
        close(pollFds[i].fd);
        memset(&pollFds[i], -1, sizeof(pollFds[i]));
    }
}

int PHAddrAndPortToStr(struct sockaddr* addr, char* addrStr, uint32_t addrStrLen, char* portStr, uint32_t portStrLen)
{
    if(addr->sa_family == AF_INET)
    {
        const char* c = inet_ntop(AF_INET, &((struct sockaddr_in*)addr)->sin_addr, addrStr, addrStrLen);
        if(c == NULL)
        {
            fprintf(stderr, "AF_INET inet_ntop() error\n"); 
            return -1;
        }

        int res = snprintf(portStr, portStrLen, "%u", ntohs(((struct sockaddr_in*)addr)->sin_port));
        if(!(res >= 0 && res < portStrLen))
        {
            fprintf(stderr, "AF_INET snprintf() error\n"); 
            return -1;
        }
    }
    else if(addr->sa_family == AF_INET6)
    {
        const char* c = inet_ntop(AF_INET6, &((struct sockaddr_in6*)addr)->sin6_addr, addrStr, addrStrLen);
        if(c == NULL)
        {
            fprintf(stderr, "AF_INET6 inet_ntop() error\n"); 
            return -2;
        }

        int res = snprintf(portStr, portStrLen, "%d", htons(((struct sockaddr_in6*)addr)->sin6_port));
        if(!(res >= 0 && res < portStrLen))
        {
            fprintf(stderr, "AF_INET6 snprintf() error\n"); 
            return -2;
        }
    } 
    else
    {
        fprintf(stderr, "invalid address family\n"); 
        return -3;
    }

    return 0;
}

int PHAddrToStr(struct sockaddr* addr, char* addrStr, uint32_t addrStrLen)
{
    if(addr->sa_family == AF_INET)
    {
        const char* c = inet_ntop(AF_INET, &((struct sockaddr_in*)addr)->sin_addr, addrStr, addrStrLen);
        if(c == NULL)
        {
            fprintf(stderr, "AF_INET inet_ntop() error\n"); 
            return -1;
        }
    }
    else if(addr->sa_family == AF_INET6)
    {
        const char* c = inet_ntop(AF_INET6, &((struct sockaddr_in6*)addr)->sin6_addr, addrStr, addrStrLen);
        if(c == NULL)
        {
            fprintf(stderr, "AF_INET6 inet_ntop() error\n"); 
            return -2;
        }
    } 
    else
    {
        fprintf(stderr, "invalid address family\n"); 
        return -3;
    }

    return 0;
}

// TODO: maybe shut down automatically if no clients are connected for a certain period of time?
/*  NOTE: 
    pollTimeOut -1  = block until event (packet received) occurs
    pollTimeOut 0   = return immediately
    pollTimeOut >0  = wait specified amount of milliseconds before returning

    loop TRUE(1)    = after a poll event keep looping while receiving packets, puts the PHRun() in an infinite loop
    loop FALSE(0)   = after a poll event return from PHRun()
*/
int PHRun(struct InitData* initData, bool32 loop, int32_t pollTimeOut, PacketReceivedCallback cb)
{
    if(initData->pollFds[LISTENER_POLLFD_INDEX].fd < 0)
    {
        fprintf(stderr, "cannot run, invalid socket\n");
        return -1;
    }

    if(loop == TRUE)
        PHReceiveLoop(initData->pollFds, sizeof(initData->pollFds) / sizeof(initData->pollFds[0]), pollTimeOut, 
            initData->packetBuffer, PACKET_BUFFER_LEN, cb);
    else if(loop == FALSE)
        PHReceiveNoLoop(initData->pollFds, sizeof(initData->pollFds) / sizeof(initData->pollFds[0]), pollTimeOut, 
            initData->packetBuffer, PACKET_BUFFER_LEN, cb);

    return 0;
}

void PHQuit(struct InitData* initData)
{
    PHRemoveAndCloseAllFromPollArray(initData->pollFds, MAX_CLIENTS + 1);
}