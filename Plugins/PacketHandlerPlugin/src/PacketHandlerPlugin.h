#include <gz/sim/System.hh>

struct InitData;
 
namespace packet_handler
{
  class PacketHandlerPlugin:
    // This class is a system.
    public gz::sim::System,
    public gz::sim::ISystemPreUpdate,
    public gz::sim::ISystemUpdate,
    public gz::sim::ISystemPostUpdate,
    public gz::sim::ISystemReset
  {
    public:
    PacketHandlerPlugin();
    ~PacketHandlerPlugin() override;
    
    void PreUpdate(const gz::sim::UpdateInfo &_info, gz::sim::EntityComponentManager &_ecm) override;
    void Update(const gz::sim::UpdateInfo &_info, gz::sim::EntityComponentManager &_ecm) override;
    void PostUpdate(const gz::sim::UpdateInfo &_info, const gz::sim::EntityComponentManager &_ecm) override;
    void Reset(const gz::sim::UpdateInfo &_info, gz::sim::EntityComponentManager &_ecm) override;

    private:
    const char* port = "8445";
    InitData* initData;
  };
}