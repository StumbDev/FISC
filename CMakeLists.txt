cmake_minimum_required(VERSION 3.10)
project(FiscConfig)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Platform-specific settings
if(WIN32)
    add_definitions(-D_WIN32_WINNT=0x0601)  # Target Windows 7 or later
    set(PLATFORM_SUFFIX ".exe")
elseif(APPLE)
    set(PLATFORM_SUFFIX "")
    set(CMAKE_OSX_ARCHITECTURES "x86_64;arm64") # Universal binary
else()
    set(PLATFORM_SUFFIX "")
endif()

# Optional Qt5
find_package(Qt5 COMPONENTS Widgets QUIET)
# Optional Curses
find_package(Curses QUIET)

# Common library
add_library(fiscconfig
    src/config/FiscConfigParser.cpp
    src/config/FiscConfigSchema.cpp
)

# CLI application (always built)
add_executable(fisc_cli
    src/cli/fisc_cli.cpp
    src/shell/MiniBiosShell.cpp
    src/vpu/FiscVpu.cpp
)
target_link_libraries(fisc_cli
    PRIVATE
    fiscconfig
)

# Set output names with platform-specific suffixes
set_target_properties(fisc_cli PROPERTIES
    OUTPUT_NAME "fisc_cli${PLATFORM_SUFFIX}"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)

# Installation rules
install(TARGETS fisc_cli
    RUNTIME DESTINATION bin
)

# Package generation
include(InstallRequiredSystemLibraries)
set(CPACK_PACKAGE_VERSION_MAJOR "1")
set(CPACK_PACKAGE_VERSION_MINOR "0")
set(CPACK_PACKAGE_VERSION_PATCH "0")
set(CPACK_PACKAGE_CONTACT "stumbledeveloper@hotmail.com")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "FISC-V Virtual Processing Unit")

if(WIN32)
    set(CPACK_GENERATOR "ZIP;NSIS")
    set(CPACK_NSIS_MODIFY_PATH ON)
elseif(APPLE)
    set(CPACK_GENERATOR "DragNDrop;TGZ")
else()
    set(CPACK_GENERATOR "DEB;RPM;TGZ")
    set(CPACK_DEBIAN_PACKAGE_DEPENDS "libc6 (>= 2.17)")
endif()

include(CPack) 