cmake_minimum_required(VERSION 3.21)
project(multi_camera_demo)

# Custom executable name.
set(EXECUTABLE_NAME CAM)

# Custom library paths.
set(MVS_PATH /opt/MVS)
set(DH_GALAXY_PATH /home/trantuan/Galaxy_camera)

# Common compile options.
add_compile_options(
        -std=c++2a
        -DAUTO_INITIALIZE_EASYLOGGINGPP
        -DELPP_THREAD_SAFE
        -DELPP_UNICODE
        # Refer to https://github.com/amrayn/easyloggingpp#logging-flags.
        -DELPP_DEFAULT_LOGGING_FLAGS=33093
)

# Options for different build type.
if (CMAKE_BUILD_TYPE STREQUAL Debug)
    add_compile_options(
            # Complete debugger support.
            -O1
            # Default log file path.
            -DELPP_DEFAULT_LOG_FILE="../log/debug.log"
            # Enable stack backtrace log when the program crashes.
            -DELPP_FEATURE_CRASH_LOG
            # Enable performance tracking macro: TIMED_SCOPE, TIMED_FUNC, etc.
            -DELPP_FEATURE_PERFORMANCE_TRACKING
    )
elseif (CMAKE_BUILD_TYPE STREQUAL Release)
    add_compile_options(
            # Maximum speed.
            -O3
            -flto
            # Disable this flag when error occurs.
            -march=native
            # Default log file path.
            -DELPP_DEFAULT_LOG_FILE="../log/release.log"
            # Disable LOG(DEBUG) to improve performance.
            -DELPP_DISABLE_DEBUG_LOGS
            # Fresh log file.
            -DELPP_FRESH_LOG_FILE
    )
endif ()

# Avoid syntax error from g++ 11.
set(CMAKE_CXX_COMPILER /usr/bin/g++-9)

# OpenCV 4.
find_package(OpenCV 4 REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})

# DH Galaxy.
include_directories(${DH_GALAXY_PATH}/inc)
file(GLOB DH_LIBS ${DH_GALAXY_PATH}/lib/x86_64/*.so)

# MVS.
include_directories(${MVS_PATH}/include)
file(GLOB MVS_LIBS ${MVS_PATH}/lib/64/*.so)

# GNU Linux.
include_directories(/usr/include/x86_64-linux-gnu)

# Include project files.
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

# Project sources.
file(GLOB SRC ${CMAKE_CURRENT_SOURCE_DIR}/modules/*/*.c*)

# Third party sources.
file(GLOB THIRD_PARTY_SRC ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/*/*.c*)

# Executable file.
add_executable(${EXECUTABLE_NAME} main.cpp ${SRC} ${THIRD_PARTY_SRC})

target_link_libraries(${EXECUTABLE_NAME}
        ${DH_LIBS}
        ${MVS_LIBS}
        ${OpenCV_LIBS}
        ${CMAKE_THREAD_LIBS_INIT}
        )
