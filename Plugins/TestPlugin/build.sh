#!/bin/bash

# library location
LIB_PATH="lib/libtestplugin.so"

# obj file location
OBJ_PATH="build/TestPlugin.o"

# source code location
CODE="src/TestPlugin.cpp"

# need to tell g++ where to find gazebo headers 
# NOTE: add them to g++ command with -I arg as needed, for example: -I$GZ_MSGS_PATH (or just use GZ_INCLUDE_ALL_HEADERS)
# NOTE: edit version numbers if needed
# TODO: determine the version numbers automatically
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

# need to tell g++ which libraries to use
LIBS="-lgz-msgs10 -lsdformat14 -lgz-sim8 -lgz-common5 -lgz-utils2 -lgz-math7 -lgz-transport13 -lgz-plugin2"

# build object files
mkdir -p build
g++ -std=c++17 -Wall -c -fPIC $CODE -o $OBJ_PATH\
    -I$GZ_SIM_PATH -I$GZ_MSGS_PATH -I$GZ_COMMON_PATH -I$GZ_UTILS_PATH\
    -I$GZ_MATH_PATH -I$GZ_TRANSPORT_PATH -I$GZ_SDFORMAT_PATH -I$GZ_PLUGIN_PATH

# create shared library
mkdir -p lib
g++ -shared -o $LIB_PATH $OBJ_PATH $LIBS