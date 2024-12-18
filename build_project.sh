#!/bin/bash

BUILD_DIR=~/Documents/research/commercial/bytev5/build

echo "Removing old build directory..."
rm -rf $BUILD_DIR

echo "Running CMake configuration..."
cmake -G Ninja \
      -DCMAKE_BUILD_TYPE=Debug \
      -DCMAKE_CXX_STANDARD=17 \
      -DCMAKE_C_COMPILER=/usr/bin/gcc \
      -DCMAKE_CXX_COMPILER=/usr/bin/g++ \
      -S ~/Documents/research/commercial/bytev5 \
      -B $BUILD_DIR

echo "Building the project..."
cmake --build $BUILD_DIR

echo "Build process completed!"
