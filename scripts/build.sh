#!/bin/bash

# Create build directory
mkdir -p build
cd build

# Configure
cmake ..

# Build
cmake --build . --config Release

# Package
cpack -G TGZ 