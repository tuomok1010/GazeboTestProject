#!/bin/bash

# executable locations
BUILD_PATH="build/TopicHandler"

# source code location
CODE_NODE="src/TopicHandler.cpp"

# need to tell g++ where to find gazebo headers
GZ_MSGS_PATH="/usr/include/gz/msgs10/"
GZ_TRANSPORT_PATH="/usr/include/gz/transport13/"
GZ_MATH_PATH="/usr/include/gz/math7/"
GZ_UTILS_PATH="/usr/include/gz/utils2/"

# need to tell g++ where to find PacketHandler header and lib file
PH_INC_PATH="../PacketHandler/include/"
PH_LIB_PATH="../PacketHandler/lib/"

# need to tell g++ which libraries to use
LIBS="-l:libprotobuf.a -lgz-msgs10 -lgz-transport13 -lgz-math7 -lgz-utils2 -lpackethandler"

mkdir -p build
g++ -Wall -o $BUILD_PATH $CODE_NODE -L$PH_LIB_PATH -I$GZ_MSGS_PATH -I$GZ_TRANSPORT_PATH -I$GZ_MATH_PATH -I$GZ_UTILS_PATH -I$PH_INC_PATH $LIBS