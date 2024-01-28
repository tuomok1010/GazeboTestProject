#ifndef PACKET_HANDLER_H
#define PACKET_HANDLER_H
#include <stdint.h>

#define MAX_CLIENTS             20U
#define LISTENER_POLLFD_INDEX   0U

#define TRUE                    1
#define FALSE                   0

#define CLIENT_INFO_ADDR_STR_LEN    46
#define CLIENT_INFO_PORT_STR_LEN    32

struct ClientInfo
{
    char addrStr[CLIENT_INFO_ADDR_STR_LEN];
    char portStr[CLIENT_INFO_PORT_STR_LEN];
};

struct pollfd;
struct sockaddr;
struct sockaddr_storage;

typedef void (*PacketReceivedCallback)(char* payload, int32_t nBytes, struct ClientInfo* clientInfo);
typedef uint32_t bool32;


int PHInit(const char* port);
int PHAcceptConnection(int listeningSocket);
int PHReceive(int socketFd, PacketReceivedCallback cb);
int PHReceiveLoop(struct pollfd* pollFds, uint32_t nPollFds, PacketReceivedCallback cb);
int PHAddToPollArray(struct pollfd* pollFds, uint32_t nPollFds, int socketToAdd);
int PHRemoveAndCloseFromPollArray(struct pollfd* pollFds, uint32_t nPollFds, int32_t socketToRemove);
void PHRemoveAndCloseAllFromPollArray(struct pollfd* pollFds, uint32_t nPollFds);
int PHAddrAndPortToStr(struct sockaddr* addr, char* addrStr, uint32_t addrStrLen, char* portStr, uint32_t portStrLen);
int PHAddrToStr(struct sockaddr* addr, char* addrStr, uint32_t addrStrLen);
int PHRun(int32_t listeningSocket, PacketReceivedCallback cb);

#endif