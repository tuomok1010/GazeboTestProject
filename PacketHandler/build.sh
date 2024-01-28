#!/bin/bash

# library location
LIB_PATH="lib/libpackethandler.a"

# obj file location
OBJ_PATH="build/PacketHandler.o"

# source code location
CODE="src/PacketHandler.c"

mkdir -p lib
mkdir -p include
mkdir -p build

gcc -Wall -c $CODE -o $OBJ_PATH
ar rcs $LIB_PATH $OBJ_PATH