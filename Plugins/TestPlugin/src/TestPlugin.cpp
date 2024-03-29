#include "TestPlugin.h"
#include <gz/plugin/Register.hh>

GZ_ADD_PLUGIN(
    test_plugin::TestPlugin,
    gz::sim::System,
    test_plugin::TestPlugin::ISystemPreUpdate,
    test_plugin::TestPlugin::ISystemUpdate,
    test_plugin::TestPlugin::ISystemPostUpdate,
    test_plugin::TestPlugin::ISystemReset)

test_plugin::TestPlugin::TestPlugin()
{
    gzmsg << "TestPlugin::TestPlugin()" << std::endl;
}

test_plugin::TestPlugin::~TestPlugin()
{
    gzmsg << "TestPlugin::~TestPlugin()" << std::endl;
}

void test_plugin::TestPlugin::PreUpdate(const gz::sim::UpdateInfo &_info, gz::sim::EntityComponentManager &_ecm)
{

}

void test_plugin::TestPlugin::Update(const gz::sim::UpdateInfo &_info, gz::sim::EntityComponentManager &_ecm)
{

}   

void test_plugin::TestPlugin::PostUpdate(const gz::sim::UpdateInfo &_info, const gz::sim::EntityComponentManager &_ecm)
{

}

void test_plugin::TestPlugin::Reset(const gz::sim::UpdateInfo &_info, gz::sim::EntityComponentManager &_ecm)
{

}