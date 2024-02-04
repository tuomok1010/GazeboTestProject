#include "PacketHandlerPlugin.h"
#include <PacketHandlerInclude.h>
#include <gz/plugin/Register.hh>
#include <gz/transport.hh>

GZ_ADD_PLUGIN(
    packet_handler::PacketHandlerPlugin,
    gz::sim::System,
    packet_handler::PacketHandlerPlugin::ISystemPreUpdate,
    packet_handler::PacketHandlerPlugin::ISystemUpdate,
    packet_handler::PacketHandlerPlugin::ISystemPostUpdate,
    packet_handler::PacketHandlerPlugin::ISystemReset)

/*
 * gets called every time a new packet has been received 
*/

void PacketHandlerReceiveCallback(char* payload, int32_t nBytes, struct ClientInfo* clientInfo)
{
    payload[nBytes] = '\0';
    printf("-------\npacket from IP: %s Port: %s Size: %d\n", clientInfo->addrStr, clientInfo->portStr, nBytes);
    printf("packet data:\n\"%s\"\n", payload);

    // convert to a topic Gazebo can understand, data from Arduino uses ':' as a delimiter between a topic and a value
    // TODO: find out if it's possible and faster/easier to send data with MQTT from Arduino
    std::string payloadStr = std::string(payload);
    std::string topicStr = payloadStr.substr(0, payloadStr.find(":"));
    std::string valueStr = payloadStr.substr(payloadStr.find(":") + 1, payloadStr.length());
    std::cout << "topicStr: " << topicStr << "\n";
    std::cout << "valueStr: " << valueStr << "\n";

    gz::transport::Node node;
    gz::transport::v13::Node::Publisher pub;

    // if we want to publish different kinds of message types based on the topic 
    // TODO: find a faster way to do it than str.compare, maybe something from std::unordered_map?
    if(topicStr.compare(std::string("/led01")) == 0)
    {
      // TODO: is doing a conversion with stringstream fast/safe?
      std::stringstream ss;
      ss << valueStr;
      uint32_t value = 0; 
      ss >> value; 

      gz::msgs::Boolean gzValue;
      gzValue.set_data(value);

      std::cout << "gzValue: " << gzValue.data() << "\n-------" << std::endl;
      pub = node.Advertise<gz::msgs::Boolean>(topicStr);
      pub.Publish(gzValue);
    }
}

packet_handler::PacketHandlerPlugin::PacketHandlerPlugin()
{
    gzmsg << "PacketHandlerPlugin::PacketHandlerPlugin()" << std::endl;

    initData = new InitData;
    PHInit(initData, port);
}

packet_handler::PacketHandlerPlugin::~PacketHandlerPlugin()
{
    gzmsg << "PacketHandlerPlugin::~PacketHandlerPlugin()" << std::endl;
    
    PHQuit(initData);
    if(initData)
        delete initData;
}

void packet_handler::PacketHandlerPlugin::PreUpdate(const gz::sim::UpdateInfo &_info, gz::sim::EntityComponentManager &_ecm)
{

}

void packet_handler::PacketHandlerPlugin::Update(const gz::sim::UpdateInfo &_info, gz::sim::EntityComponentManager &_ecm)
{
    if(initData)
        PHRun(initData, FALSE, 0, PacketHandlerReceiveCallback);
}   

void packet_handler::PacketHandlerPlugin::PostUpdate(const gz::sim::UpdateInfo &_info, const gz::sim::EntityComponentManager &_ecm)
{

}

void packet_handler::PacketHandlerPlugin::Reset(const gz::sim::UpdateInfo &_info, gz::sim::EntityComponentManager &_ecm)
{
    gzmsg << "TestPlugin::Reset()" << std::endl;
}