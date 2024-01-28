#include <gz/msgs.hh>
#include <gz/transport.hh>
#include <PacketHandlerInclude.h>

#define PORT  "8445"


/*
 * gets called every time a new packet has been received 
*/
void PacketHandlerReceiveCallback(char* payload, int32_t nBytes, struct ClientInfo* clientInfo)
{
    payload[nBytes] = '\0';
    printf("packet from IP: %s Port: %s\n", clientInfo->addrStr, clientInfo->portStr);
    printf("packet size: %d bytes\n", nBytes);
    printf("packet data:\n\"%s\"\n", payload);

    // convert to a topic Gazebo can understand, data from Arduino uses ':' as a delimiter between a topic and a value
    std::string topic = std::string(payload).substr(0, std::string(payload).find(":"));
    std::string value = std::string(payload).substr(std::string(payload).find(":") + 1, std::string(payload).length());
    std::cout << "received topic: " << topic << std::endl;
    std::cout << "topic value: " << value << std::endl;

    gz::transport::Node node;
    gz::transport::v13::Node::Publisher pub = node.Advertise<gz::msgs::StringMsg>(topic);
    topics[topic] = pub;
    pub.Publish(value);
}

//////////////////////////////////////////////////
int main(int argc, char **argv)
{
  int listeningSocket = PHInit(PORT);
  PHRun(listeningSocket, PacketHandlerReceiveCallback);
  return 0;
}