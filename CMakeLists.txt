cmake_minimum_required(VERSION 3.12)
project(xtalk_dev VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

file(GLOB SRC_FILES "src/*.cpp")

add_executable(xtalk_dev ${SRC_FILES})

target_include_directories(xtalk_dev PRIVATE /opt/vosk-api/src include)

target_link_libraries(xtalk_dev PRIVATE /opt/vosk-api/src/libvosk.so zmq SDL2 SDL2_mixer log4cpp)