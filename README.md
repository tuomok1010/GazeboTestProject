# GazeboTestProject
Testing/Learning the Gazebo simulation software with Arduino Wifi Rev2. This is a work in progress and a personal sandbox project for learning Gazebo.


1. Install Gazebo Harmonic with the binary install method

2. Install Arduino IDE

3. Create a "network_data.h" file in the Arduino/LED_Project folder. It should contain the following lines:
#define SSID_NAME "INSERT_YOUR_SSID"
#define PASS "INSERT_PASSWORD"

4. In Arduino/LED_Project.ino, change the targetAddr variable(line 10) to the address of the host Gazebo is running on

5. Compile and upload the Arduino/LED_Project.ino into your Arduino Wifi Rev2. Note: update firmware first

6. Run PacketHandler/build_both.sh

7. Navigate to the "World" folder and set the Gazebo server config environment variable with the following command(NOTE: edit the path if necessary): 
export GZ_SIM_SERVER_CONFIG_PATH=$HOME/Documents/Gazebo/TestProject/World/test_world_server.config

8. Edit the "test_world_server.config" file, so that the TestPlugin element's "filename" attribute points to where the TestPlugin library is located in your system.

9. Edit the "test_world_server.config" file, so that the PacketHandlerPlugin element's "filename" attribute points to where the PacketHandlerPlugin library is located in your system.

10. Run World/test_project_launch.gzlaunch with the command "gz launch test_project_launch.gzlaunch"

11. If everything works, the PacketHandlerPlugin is listening on port "8445" for tcp traffic. Any packets are received in the PacketHandlerReceiveCallback() function. In the example code, an Arduino is sending LED status tcp message and the PacketHandlerPlugin program converts it to a gz::msgs::Boolean message and publishes it.



TODOs/Notes to self: 
- Make the setup above more simple
- Do some more complex stuff with the Arduino, send data from Gazebo to the Arduino
- Check if it's possible to use an Arduino MQTT library with Gazebo
- Visualize Arduino in the Gazebo viewport