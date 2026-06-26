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
unset(PROTO_REPO_PATH CACHE)
set(PROTO_REPO_PATH "" CACHE PATH "Local path to external protobuf repository")
unset(PROTO_REPO_GIT_URL CACHE)
set(PROTO_REPO_GIT_URL "https://github.com/oxfrd/silo-sensors-proto-msg.git" CACHE STRING "Git URL for external protobuf repository if PROTO_REPO_PATH is not provided")
unset(PROTO_REPO_GIT_TAG CACHE)
set(PROTO_REPO_GIT_TAG "master" CACHE STRING "Git tag/branch to checkout when fetching external protobuf repository")

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

find_package(Protobuf REQUIRED)
find_package(Git REQUIRED)

# Filesystem support (for C++17)
find_package(Threads REQUIRED)

# ============================================================================
# Source Files
# ============================================================================
set(CORE_SOURCES
    src/core/monitoringService.cpp
)

set(SENSORS_SOURCES
    src/sensors/sensorManager.cpp
    src/sensors/mockSensorProvider.cpp
    src/sensors/realSensorProvider.cpp
    src/sensors/deltaSensorFilter.cpp
    src/sensors/movingAverageOutlierRejectingSensorFilter.cpp
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

set(COMMUNICATION_SOURCES
    src/communication/udsServer.cpp
)

# External protobuf definitions repository
get_filename_component(PROJECT_PARENT_DIR ${CMAKE_CURRENT_SOURCE_DIR} DIRECTORY)
set(PROTO_REPO_DEFAULT_PATH "${PROJECT_PARENT_DIR}/silo-sensors-proto-msg")

if(NOT "${PROTO_REPO_PATH}" STREQUAL "")
    set(PROTO_REPO_DIR "${PROTO_REPO_PATH}")
    set(PROTO_SNAPSHOT_FILE "${PROTO_REPO_DIR}/proto/snapshot.proto")
    set(PROTO_ALARMS_FILE "${PROTO_REPO_DIR}/proto/alarms.proto")
    message(STATUS "Using external protobuf definition: ${PROTO_SNAPSHOT_FILE}")
    message(STATUS "Using external protobuf definition: ${PROTO_ALARMS_FILE}")
    protobuf_generate_cpp(SNAPSHOT_PROTO_SRCS SNAPSHOT_PROTO_HDRS ${PROTO_SNAPSHOT_FILE})
    protobuf_generate_cpp(ALARMS_PROTO_SRCS ALARMS_PROTO_HDRS ${PROTO_ALARMS_FILE})

    set(PROTO_SRCS
        ${SNAPSHOT_PROTO_SRCS}
        ${ALARMS_PROTO_SRCS}
    )
    set(PROTO_HDRS
        ${SNAPSHOT_PROTO_HDRS}
        ${ALARMS_PROTO_HDRS}
    )
elseif(NOT "${PROTO_REPO_GIT_URL}" STREQUAL "")
    set(PROTO_REPO_DIR "${CMAKE_CURRENT_BINARY_DIR}/silo-sensors-proto-msg")
    if(NOT EXISTS "${PROTO_REPO_DIR}/.git")
        if(PROTO_REPO_GIT_TAG AND NOT "${PROTO_REPO_GIT_TAG}" STREQUAL "")
            message(STATUS "Cloning external protobuf repository from ${PROTO_REPO_GIT_URL} (tag ${PROTO_REPO_GIT_TAG})")
            execute_process(
                COMMAND "${GIT_EXECUTABLE}" clone --depth 1 --branch "${PROTO_REPO_GIT_TAG}" "${PROTO_REPO_GIT_URL}" "${PROTO_REPO_DIR}"
                RESULT_VARIABLE PROTO_GIT_CLONE_RESULT
                OUTPUT_VARIABLE PROTO_GIT_CLONE_OUTPUT
                ERROR_VARIABLE PROTO_GIT_CLONE_ERROR
            )
        else()
            message(STATUS "Cloning external protobuf repository from ${PROTO_REPO_GIT_URL} using the remote default branch")
            execute_process(
                COMMAND "${GIT_EXECUTABLE}" clone --depth 1 "${PROTO_REPO_GIT_URL}" "${PROTO_REPO_DIR}"
                RESULT_VARIABLE PROTO_GIT_CLONE_RESULT
                OUTPUT_VARIABLE PROTO_GIT_CLONE_OUTPUT
                ERROR_VARIABLE PROTO_GIT_CLONE_ERROR
            )
        endif()
        if(NOT PROTO_GIT_CLONE_RESULT EQUAL 0)
            message(FATAL_ERROR "Failed to clone protobuf repository: ${PROTO_GIT_CLONE_ERROR}")
        endif()
    endif()
    set(PROTO_SNAPSHOT_FILE "${PROTO_REPO_DIR}/proto/snapshot.proto")
    set(PROTO_ALARMS_FILE "${PROTO_REPO_DIR}/proto/alarms.proto")
    message(STATUS "Using external protobuf definition: ${PROTO_SNAPSHOT_FILE}")
    message(STATUS "Using external protobuf definition: ${PROTO_ALARMS_FILE}")
    protobuf_generate_cpp(SNAPSHOT_PROTO_SRCS SNAPSHOT_PROTO_HDRS ${PROTO_SNAPSHOT_FILE})
    protobuf_generate_cpp(ALARMS_PROTO_SRCS ALARMS_PROTO_HDRS ${PROTO_ALARMS_FILE})

    set(PROTO_SRCS
        ${SNAPSHOT_PROTO_SRCS}
        ${ALARMS_PROTO_SRCS}
    )
    set(PROTO_HDRS
        ${SNAPSHOT_PROTO_HDRS}
        ${ALARMS_PROTO_HDRS}
    )
elseif(EXISTS "${PROTO_REPO_DEFAULT_PATH}")
    set(PROTO_REPO_DIR "${PROTO_REPO_DEFAULT_PATH}")
    set(PROTO_SNAPSHOT_FILE "${PROTO_REPO_DIR}/proto/snapshot.proto")
    set(PROTO_ALARMS_FILE "${PROTO_REPO_DIR}/proto/alarms.proto")
    message(STATUS "Using external protobuf definition: ${PROTO_SNAPSHOT_FILE}")
    message(STATUS "Using external protobuf definition: ${PROTO_ALARMS_FILE}")
    protobuf_generate_cpp(SNAPSHOT_PROTO_SRCS SNAPSHOT_PROTO_HDRS ${PROTO_SNAPSHOT_FILE})
    protobuf_generate_cpp(ALARMS_PROTO_SRCS ALARMS_PROTO_HDRS ${PROTO_ALARMS_FILE})

    set(PROTO_SRCS
        ${SNAPSHOT_PROTO_SRCS}
        ${ALARMS_PROTO_SRCS}
    )
    set(PROTO_HDRS
        ${SNAPSHOT_PROTO_HDRS}
        ${ALARMS_PROTO_HDRS}
    )
elseif(EXISTS "${PROTO_REPO_DEFAULT_PATH}")
    set(PROTO_REPO_DIR "${PROTO_REPO_DEFAULT_PATH}")
    set(PROTO_SNAPSHOT_FILE "${PROTO_REPO_DIR}/proto/snapshot.proto")
    set(PROTO_ALARMS_FILE "${PROTO_REPO_DIR}/proto/alarms.proto")
    message(STATUS "Using external protobuf definition: ${PROTO_SNAPSHOT_FILE}")
    message(STATUS "Using external protobuf definition: ${PROTO_ALARMS_FILE}")
    protobuf_generate_cpp(SNAPSHOT_PROTO_SRCS SNAPSHOT_PROTO_HDRS ${PROTO_SNAPSHOT_FILE})
    protobuf_generate_cpp(ALARMS_PROTO_SRCS ALARMS_PROTO_HDRS ${PROTO_ALARMS_FILE})

    set(PROTO_SRCS
        ${SNAPSHOT_PROTO_SRCS}
        ${ALARMS_PROTO_SRCS}
    )
    set(PROTO_HDRS
        ${SNAPSHOT_PROTO_HDRS}
        ${ALARMS_PROTO_HDRS}
    )
else()
    message(FATAL_ERROR "External protobuf repository not found. Set PROTO_REPO_PATH, add sibling repo 'silo-sensors-proto-msg', or set PROTO_REPO_GIT_URL.")
endif()

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
    ${COMMUNICATION_SOURCES}
    ${PROTO_SRCS}
)

# ============================================================================
# Header Files
# ============================================================================
set(CORE_HEADERS
    include/core/monitoringService.h
)

set(SENSORS_HEADERS
    include/sensors/sensorInterface.h
    include/sensors/sensorManager.h
    include/sensors/sensorData.h
    include/sensors/mockSensorProvider.h
    include/sensors/realSensorProvider.h
    include/sensors/iSensorManager.h
    include/sensors/iSensorFilter.h
    include/sensors/deltaSensorFilter.h
    include/sensors/movingAverageOutlierRejectingSensorFilter.h
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

set(COMMUNICATION_HEADERS
    include/communication/iSnapshotProvider.h
    include/communication/udsServer.h
)

set(ALL_HEADERS
    ${CORE_HEADERS}
    ${SENSORS_HEADERS}
    ${ALARMS_HEADERS}
    ${ASSIGNMENTS_HEADERS}
    ${HISTORY_HEADERS}
    ${COMMUNICATION_HEADERS}
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
    ${CMAKE_CURRENT_SOURCE_DIR}/include/communication
    ${JSONCPP_INCLUDE_DIRS}
    ${Protobuf_INCLUDE_DIRS}
    ${CMAKE_CURRENT_BINARY_DIR}
)

# ============================================================================
# Link Libraries
# ============================================================================
target_link_libraries(silo-monitor-sensors-app PRIVATE
    ${JSONCPP_LIBRARIES}
    ${Protobuf_LIBRARIES}
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
