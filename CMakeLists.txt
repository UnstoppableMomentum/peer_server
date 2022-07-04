###############################
#                             #
# Copyright (c) 2022 Selenika #
#                             #
###############################

cmake_minimum_required(VERSION 3.10)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(Boost_USE_STATIC_LIBS ON)


add_definitions(-DDEBUG)

# set the project name
project(selenika-peer-server)

include_directories(${CMAKE_SOURCE_DIR})
include_directories(options)

find_package(Boost 1.77.0 COMPONENTS log regex program_options thread system REQUIRED)
find_package(OpenSSL)
find_package(Threads REQUIRED)

# add the executable
add_executable(selenika-peer-server
        main.cpp
        listener.cpp
        shared_state.cpp
        websocket_session.cpp
        config/logging.cpp
        messaging/response.cpp
        options/cmd_line_options.cpp
        )

target_link_libraries(selenika-peer-server ${OPENSSL_LIBRARIES} -static-libgcc -static-libstdc++)
target_link_libraries(selenika-peer-server ${Boost_LIBRARIES} -static-libgcc -static-libstdc++)
target_link_libraries(selenika-peer-server ${CMAKE_THREAD_LIBS_INIT} -static-libgcc -static-libstdc++)
