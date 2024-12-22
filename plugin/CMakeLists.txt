cmake_minimum_required(VERSION 3.12)
project(xtalk_plugin VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_library(xtalk_plugin SHARED xtalk_plugin.cpp ../xtalk_dev_1/src/zmq_server.cpp)

target_include_directories(xtalk_plugin PRIVATE /opt/vosk-api/src ${CMAKE_SOURCE_DIR}/SDK/CHeaders/XPLM ${CMAKE_SOURCE_DIR}/../xtalk_dev_1/include)

target_link_libraries(xtalk_plugin PRIVATE zmq log4cpp /mnt/sdb1/Sim/XP12/Resources/plugins/XPLM_64.so)

target_compile_definitions(xtalk_plugin PRIVATE APL=3)

set_target_properties(xtalk_plugin PROPERTIES PREFIX "")

set_target_properties(xtalk_plugin PROPERTIES SUFFIX ".xpl")