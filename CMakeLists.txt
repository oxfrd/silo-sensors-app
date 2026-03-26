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
    set(SENSOR_PROVIDER_SOURCE "src/sensors/realSensorProvider.cpp")
else()
    message(STATUS "Using MOCK SENSOR provider")
    set(SENSOR_PROVIDER_SOURCE "src/sensors/mockSensorProvider.cpp")
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
set(CORE_SOURCES
    src/core/monitoringService.cpp
    src/core/temperatureMonitor.cpp
)

set(SENSORS_SOURCES
    src/sensors/sensorManager.cpp
    src/sensors/sensorValidator.cpp
    src/sensors/mockSensorProvider.cpp
    src/sensors/realSensorProvider.cpp
)

set(ALARMS_SOURCES
    src/alarms/alarmManager.cpp
)

set(ASSIGNMENTS_SOURCES
    src/assignments/assignmentsManager.cpp
)

set(HISTORY_SOURCES
    src/history/historyRecorder.cpp
)

set(MAIN_SOURCES
    src/main.cpp
)

set(ALL_SOURCES
    ${MAIN_SOURCES}
    ${CORE_SOURCES}
    ${SENSORS_SOURCES}
    ${ALARMS_SOURCES}
    ${ASSIGNMENTS_SOURCES}
    ${HISTORY_SOURCES}
)

# ============================================================================
# Header Files
# ============================================================================
set(CORE_HEADERS
    include/core/monitoringService.h
    include/core/temperatureMonitor.h
)

set(SENSORS_HEADERS
    include/sensors/sensorInterface.h
    include/sensors/sensorManager.h
    include/sensors/sensorData.h
    include/sensors/sensorValidator.h
    include/sensors/mockSensorProvider.h
    include/sensors/realSensorProvider.h
    include/sensors/iSensorManager.h
)

set(ALARMS_HEADERS
    include/alarms/iAlarmManager.h
    include/alarms/alarmManager.h
    include/alarms/alarmCodes.h
    include/alarms/sensorAlarmState.h
)

set(ASSIGNMENTS_HEADERS
    include/assignments/iAssignmentsManager.h
    include/assignments/assignmentsManager.h
)

set(HISTORY_HEADERS
    include/history/iHistoryRecorder.h
    include/history/historyRecorder.h
)

set(ALL_HEADERS
    ${CORE_HEADERS}
    ${SENSORS_HEADERS}
    ${ALARMS_HEADERS}
    ${ASSIGNMENTS_HEADERS}
    ${HISTORY_HEADERS}
)

# ============================================================================
# Executable
# ============================================================================
add_executable(silo-monitor-sensors-app ${ALL_SOURCES} ${ALL_HEADERS})

# ============================================================================
# Include Directories
# ============================================================================
target_include_directories(silo-monitor-sensors-app PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/include/core
    ${CMAKE_CURRENT_SOURCE_DIR}/include/sensors
    ${CMAKE_CURRENT_SOURCE_DIR}/include/alarms
    ${CMAKE_CURRENT_SOURCE_DIR}/include/assignments
    ${CMAKE_CURRENT_SOURCE_DIR}/include/history
    ${JSONCPP_INCLUDE_DIRS}
)

# ============================================================================
# Link Libraries
# ============================================================================
target_link_libraries(silo-monitor-sensors-app PRIVATE
    ${JSONCPP_LIBRARIES}
    Threads::Threads
    stdc++fs
)

# ============================================================================
# Compiler Flags
# ============================================================================
if(UNIX AND NOT APPLE)
    target_compile_options(silo-monitor-sensors-app PRIVATE -fPIC)
endif()

# ============================================================================
# Installation
# ============================================================================
install(TARGETS silo-monitor-sensors-app
    RUNTIME DESTINATION bin
)

install(FILES config/silo_assignments.json
    DESTINATION etc/silo-monitor
    OPTIONAL
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
