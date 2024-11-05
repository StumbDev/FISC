@echo off

:: Create build directory
if not exist build mkdir build
cd build

:: Configure
cmake ..

:: Build
cmake --build . --config Release

:: Package
cpack -G ZIP 