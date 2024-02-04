#!/bin/bash

# library location
LIB_PATH="lib/static/libpackethandler.a"

# obj file location
OBJ_PATH="build/static/PacketHandler.o"

# source code location
CODE="src/PacketHandler.c"

mkdir -p lib/static
mkdir -p include
mkdir -p build/static

gcc -Wall -c $CODE -o $OBJ_PATH
ar rcs $LIB_PATH $OBJ_PATH

echo "static library built"