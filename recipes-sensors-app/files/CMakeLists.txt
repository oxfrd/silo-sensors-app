cmake_minimum_required(VERSION 3.12)
project(SensorApp)

# ============================================================================
# Build Configuration Options
# ============================================================================
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic")

# Opcje budowania
option(USE_REAL_SENSORS "Use real DS18B20 sensors (disable for mock)" ON)
option(BUILD_DEBUG "Build with debug symbols" ON)

# Debug configuration
if(BUILD_DEBUG)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -O0")
    add_compile_definitions(DEBUG_MODE)
    message(STATUS "Building with DEBUG configuration")
else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2")
    message(STATUS "Building with RELEASE configuration")
endif()

# ============================================================================
# Sensor Provider Configuration
# ============================================================================
if(USE_REAL_SENSORS)
    message(STATUS "Using REAL SENSOR provider (DS18B20)")
    add_compile_definitions(USE_REAL_SENSORS)
    set(SENSOR_PROVIDER_SOURCE "realSensorProvider.cpp")
else()
    message(STATUS "Using MOCK SENSOR provider")
    set(SENSOR_PROVIDER_SOURCE "mockSensorProvider.cpp")
endif()

# ============================================================================
# Find Dependencies
# ============================================================================
find_package(PkgConfig QUIET)

# jsoncpp
if(PkgConfig_FOUND)
    pkg_check_modules(JSONCPP jsoncpp)
else()
    find_library(JSONCPP_LIBRARY jsoncpp REQUIRED)
    set(JSONCPP_LIBRARIES ${JSONCPP_LIBRARY})
    message(STATUS "Found jsoncpp: ${JSONCPP_LIBRARY}")
endif()

# Filesystem support (for C++17)
find_package(Threads REQUIRED)

# ============================================================================
# Source Files
# ============================================================================
set(SOURCES
    main.cpp
    sensorManager.cpp
    assignmentsManager.cpp
    realSensorProvider.cpp
    mockSensorProvider.cpp
    historyRecorder.cpp
    ${SENSOR_PROVIDER_SOURCE}
)

set(HEADERS
    sensorInterface.h
    sensorManager.h
    assignmentsManager.h
    realSensorProvider.h
    mockSensorProvider.h
    historyRecorder.h
)

# ============================================================================
# Executable
# ============================================================================
add_executable(sensor-app ${SOURCES} ${HEADERS})

# ============================================================================
# Include Directories
# ============================================================================
target_include_directories(sensor-app PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${JSONCPP_INCLUDE_DIRS}
)

# ============================================================================
# Link Libraries
# ============================================================================
target_link_libraries(sensor-app PRIVATE
    ${JSONCPP_LIBRARIES}
    Threads::Threads
    stdc++fs
)

# ============================================================================
# Compiler Flags
# ============================================================================
if(UNIX AND NOT APPLE)
    target_compile_options(sensor-app PRIVATE -fPIC)
endif()

# ============================================================================
# Installation
# ============================================================================
install(TARGETS sensor-app
    RUNTIME DESTINATION bin
)

# ============================================================================
# Summary
# ============================================================================
message(STATUS "")
message(STATUS "=== Sensor App Configuration ===")
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "C++ Standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "Sensor Provider: ${SENSOR_PROVIDER_SOURCE}")
message(STATUS "DEBUG mode: ${BUILD_DEBUG}")
message(STATUS "jsoncpp found: ${JSONCPP_LIBRARIES}")
message(STATUS "================================")
message(STATUS "")
