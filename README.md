# GazeboTestProject

0. Install Gazebo Harmonic with the binary install method
1. Edit TopicHandler/build.sh file so that the Gazebo header paths match those of your system's
2. Edit the TopicHandler/build.sh so that the LIBS variable has the same Gazebo library versions as you NOTE: if you need to change these, the program may not run or compile at all
3. Run PacketHandler/build.sh and then TopicHandler/build.sh
4. Run World/test_project_launch.gzlaunch, or just run TopicHandler/build/TopicHandler

5. If everything works, the TopicHandler (which is using the PacketHandler lib) is listening on port "8445" (default) for tcp traffic. Any packets are received in the PacketHandlerReceiveCallback() function. In the example code, an Arduino is sending LED status tcp message and the program converts it to a gz::msgs::Boolean message and publishes it. 