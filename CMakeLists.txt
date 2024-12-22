cmake_minimum_required(VERSION 3.12)
project(x-talk VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_THREAD_PREFER_PTHREAD TRUE)
set(THREADS_PREFER_PTHREAD_FLAG TRUE)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(Threads REQUIRED)

if(EXISTS "${CMAKE_BINARY_DIR}/compile_commands.json")
    file(COPY "${CMAKE_BINARY_DIR}/compile_commands.json"
         DESTINATION "${CMAKE_SOURCE_DIR}/build")
endif()

file(GLOB SRC_FILES "src/*.cpp")

if(PLATFORM STREQUAL "Linux")
add_executable(x-talk ${SRC_FILES})
target_include_directories(x-talk PRIVATE /opt/vosk-api/src include)
target_link_libraries(x-talk PRIVATE /opt/vosk-api/src/libvosk.so zmq SDL2 SDL2_mixer)
endif()

if(PLATFORM STREQUAL "Windows")
add_executable(x-talk "src/main.cpp" ${SRC_FILES})
add_definitions(-DSDL_MAIN_HANDLED -DZMQ_STATIC)
set(LIBZMQ_DIR "${CMAKE_SOURCE_DIR}/libs")
#target_compile_definitions(x-talk PRIVATE ZMQ_DLL)
target_include_directories(x-talk PRIVATE include include/third_party)
target_link_directories(x-talk PRIVATE libs)
target_link_libraries(x-talk PRIVATE Threads::Threads -Wl,-Bdynamic vosk -Wl,-Bstatic zmq SDL2 SDL2_mixer sodium ssp ole32 oleaut32 gdi32 ws2_32 winmm cfgmgr32 imm32 version setupapi iphlpapi)
endif()