cmake_minimum_required(VERSION 3.26)
project(Chat_Server)

set(CMAKE_CXX_STANDARD 11)

set(CMAKE_CXX_FLAGS {$CMAKE_CXX_FLAGS} -g)

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)

include_directories(
        ${PROJECT_SOURCE_DIR}/include
        ${PROJECT_SOURCE_DIR}/thirdparty
        ${PROJECT_SOURCE_DIR}/include/server
        ${PROJECT_SOURCE_DIR}/include/server/db
        ${PROJECT_SOURCE_DIR}/include/server/model
        ${PROJECT_SOURCE_DIR}/include/server/redis
        ${PROJECT_SOURCE_DIR}/thirdparty/my_network_demo/src/base
        ${PROJECT_SOURCE_DIR}/thirdparty/my_network_demo/src/logger
        ${PROJECT_SOURCE_DIR}/thirdparty/my_network_demo/src/net/poller
        ${PROJECT_SOURCE_DIR}/thirdparty/my_network_demo/src/net
        ${PROJECT_SOURCE_DIR}/thirdparty/my_network_demo/src/timer
)

add_subdirectory(src)