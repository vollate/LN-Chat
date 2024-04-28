#!/bin/bash

VCPKG_TOOLCHAIN=$TOOLCHAIN
BUILD_CMD="cmake -DCMAKE_TOOLCHAIN_FILE=$VCPKG_TOOLCHAIN -DCMAKE_BUILD_TYPE="

if [ $# -eq 0 ]
then
    echo "Usage: dev.sh [debug|release]"
    exit 1
fi
if [ $1 == "debug" ]
then
BUILD_CMD="${BUILD_CMD}Debug -B build-$1"
elif [ $1 == "release" ]
then
BUILD_CMD="${BUILD_CMD}Release -B build-$1"
else
echo "Usage: dev.sh [debug|release]"
exit 1
fi

eval $BUILD_CMD
cmake --build build-$1 -j
