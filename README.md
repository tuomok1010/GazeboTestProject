# GazeboTestProject
Testing/Learning the Gazebo simulation software with Arduino Wifi Rev2. This is a work in progress and a personal sandbox project for learning Gazebo.


1. Install Gazebo Harmonic with the binary install method

2. Install Arduino IDE

3. Create a "network_data.h" file in the Arduino/LED_Project folder. It should contain the following lines:
#define SSID_NAME "INSERT_YOUR_SSID"
#define PASS "INSERT_PASSWORD"

4. Compile and upload the Arduino/LED_Project.ino into your Arduino Wifi Rev2. Note: update firmware first

5. Edit TopicHandler/build.sh file so that the Gazebo header paths match those of your system's. NOTE: if you are unsure where they are located, you can try searching them by running the command "locate <filename here>". For example: "locate gz/transport.hh".

6. Edit the TopicHandler/build.sh so that the LIBS variable has the same Gazebo library versions as you NOTE: if you need to change these, the program may not run or compile at all. 

7. Run PacketHandler/build.sh and then TopicHandler/build.sh

8. Navigate to the "World" folder and set the Gazebo server config environment variable with the following command(NOTE: edit the path if necessary): 
export GZ_SIM_SERVER_CONFIG_PATH=$HOME/Documents/Gazebo/TestProject/World/test_world_server.config

9. Edit the "test_world_server.config" file, so that the TestPlugin "filename" attribute of the last plugin points to where the TestPlugin library is located in your system.

10. Run World/test_project_launch.gzlaunch with the command "gz launch test_project_launch.gzlaunch"

11. If everything works, the TopicHandler (which is using the PacketHandler lib) is listening on port "8445" (default) for tcp traffic. Any packets are received in the PacketHandlerReceiveCallback() function. In the example code, an Arduino is sending LED status tcp message and the program converts it to a gz::msgs::Boolean message and publishes it. The TestPlugin prints a message when it is constructed.

TODOs: 
- Make the setup above more simple
- Convert PacketHandler and TopicHandler into a Gazebo plugin
- Do some more complex stuff with the Arduino, send data from Gazebo to the Arduino
- Check if it's possible to use an Arduino MQTT library with Gazebo