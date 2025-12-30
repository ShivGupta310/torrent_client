cmake_minimum_required(VERSION 3.10)
project(TorrentClient)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


find_package(OpenSSL REQUIRED)


add_library(torrent_core
    src/core/bencode.cpp
    src/core/torrent.cpp
    src/io/file_handler.cpp
    src/network/tracker.cpp
)

target_link_libraries(torrent_core PRIVATE OpenSSL::SSL OpenSSL::Crypto)
add_executable(test_runner test/test_torrent.cpp)
target_link_libraries(test_runner PRIVATE torrent_core)
add_executable(test_network test/test_network.cpp)
target_link_libraries(test_network PRIVATE torrent_core)

