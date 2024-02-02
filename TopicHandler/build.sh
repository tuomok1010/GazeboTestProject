#!/bin/bash

# executable location
BUILD_PATH="build/TopicHandler"

# source code location
CODE_PATH="src/TopicHandler.cpp"

# need to tell g++ where to find gazebo headers 
# NOTE: add them to g++ command with -I arg as needed, for example: -I$GZ_MSGS_PATH (or just use GZ_INCLUDE_ALL_HEADERS)
# NOTE: edit version numbers if needed
GZ_CMAKE_PATH="/usr/include/gz/cmake3/"
GZ_COMMON_PATH="/usr/include/gz/common5/"
GZ_FUEL_TOOLS_PATH="/usr/include/gz/fuel_tools9/"
GZ_GUI_PATH="/usr/include/gz/gui8/"
GZ_LAUNCH_PATH="/usr/include/gz/launch7/"
GZ_MATH_PATH="/usr/include/gz/math7/"
GZ_MSGS_PATH="/usr/include/gz/msgs10/"
GZ_PHYSICS_PATH="/usr/include/gz/physics7/"
GZ_PLUGIN_PATH="/usr/include/gz/plugin2/"
GZ_RENDERING_PATH="/usr/include/gz/rendering8/"
GZ_SDFORMAT_PATH="/usr/include/gz/sdformat14/"
GZ_SENSORS_PATH="/usr/include/gz/sensors8/"
GZ_SIM_PATH="/usr/include/gz/sim8/"
GZ_TRANSPORT_PATH="/usr/include/gz/transport13/"
GZ_UTILS_PATH="/usr/include/gz/utils2/"

GZ_INCLUDE_ALL_HEADERS="-I$GZ_CMAKE_PATH -I$GZ_COMMON_PATH -I$GZ_FUEL_TOOLS_PATH -I$GZ_GUI_PATH\
    -I$GZ_LAUNCH_PATH -I$GZ_MATH_PATH -I$GZ_MSGS_PATH -I$GZ_PHYSICS_PATH -I$GZ_PLUGIN_PATH -I$GZ_RENDERING_PATH\
    -I$GZ_SDFORMAT_PATH -I$GZ_SENSORS_PATH -I$GZ_SIM_PATH -I$GZ_TRANSPORT_PATH -I$GZ_UTILS_PATH"

# need to tell g++ where to find PacketHandler header and lib file
PH_INC_PATH="../PacketHandler/include/"
PH_LIB_PATH="../PacketHandler/lib/"

# need to tell g++ which libraries to use
LIBS="-l:libprotobuf.a -lgz-msgs10 -lgz-transport13 -lgz-math7 -lgz-utils2 -lpackethandler"

mkdir -p build
g++ -Wall -o $BUILD_PATH $CODE_PATH -L$PH_LIB_PATH -I$GZ_MSGS_PATH -I$GZ_TRANSPORT_PATH -I$GZ_MATH_PATH -I$GZ_UTILS_PATH -I$PH_INC_PATH $LIBS