#ifndef PACKET_HANDLER_H
#define PACKET_HANDLER_H
#include <stdint.h>

#define MAX_CLIENTS             20U
#define LISTENER_POLLFD_INDEX   0U

#define TRUE                    1
#define FALSE                   0

#define CLIENT_INFO_ADDR_STR_LEN    46
#define CLIENT_INFO_PORT_STR_LEN    32

#define PACKET_BUFFER_LEN           (65535 + 1)

struct ClientInfo
{
    char addrStr[CLIENT_INFO_ADDR_STR_LEN];
    char portStr[CLIENT_INFO_PORT_STR_LEN];
};

struct InitData
{
    int32_t listeningSocket;
    char packetBuffer[PACKET_BUFFER_LEN];
};

struct pollfd;
struct sockaddr;
struct sockaddr_storage;

typedef void (*PacketReceivedCallback)(char* payload, int32_t nBytes, struct ClientInfo* clientInfo);
typedef uint32_t bool32;


int PHInit(struct InitData* initData, const char* port);
int PHAcceptConnection(int listeningSocket);
int PHReceive(int32_t socketFd, char* packetBuffer, uint32_t packetBufferSizeInBytes, PacketReceivedCallback cb);
int PHReceiveLoop(struct pollfd* pollFds, uint32_t nPollFds, char* packetBuffer, uint32_t packetBufferSizeInBytes, PacketReceivedCallback cb);
int PHAddToPollArray(struct pollfd* pollFds, uint32_t nPollFds, int socketToAdd);
int PHRemoveAndCloseFromPollArray(struct pollfd* pollFds, uint32_t nPollFds, int32_t socketToRemove);
void PHRemoveAndCloseAllFromPollArray(struct pollfd* pollFds, uint32_t nPollFds);
int PHAddrAndPortToStr(struct sockaddr* addr, char* addrStr, uint32_t addrStrLen, char* portStr, uint32_t portStrLen);
int PHAddrToStr(struct sockaddr* addr, char* addrStr, uint32_t addrStrLen);
int PHRun(struct InitData* initData, PacketReceivedCallback cb);

#endif