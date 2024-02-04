#!/bin/bash

# library location
LIB_PATH="lib/shared/libpackethandler.so"

# obj file location
OBJ_PATH="build/shared/PacketHandler.o"

# source code location
CODE="src/PacketHandler.c"

mkdir -p lib/shared
mkdir -p include
mkdir -p build/shared

# compile to position independent code obj files
gcc -Wall -c -fPIC -o $OBJ_PATH $CODE

# create the shared library
gcc -shared -o $LIB_PATH $OBJ_PATH

echo "shared library built"