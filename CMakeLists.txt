#
# Copyright (c) 2022 QAZ
#

cmake_minimum_required(VERSION 3.10)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# set the project name
project(peer_server)

find_package(Boost 1.77.0 COMPONENTS regex program_options thread system REQUIRED)
find_package(OpenSSL)
find_package(Threads REQUIRED)


include_directories(.)

# add the executable
add_executable(peer_server
        main.cpp
        listener.cpp
        shared_state.cpp
        websocket_session.cpp
        messaging/response.cpp
        )

target_link_libraries(peer_server ${Boost_LIBRARIES})
target_link_libraries(peer_server ${OPENSSL_LIBRARIES})
target_link_libraries(peer_server ${CMAKE_THREAD_LIBS_INIT})
