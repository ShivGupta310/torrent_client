#include "../src/core/torrent.hpp"
#include "../src/network/tracker.hpp"
#include "../src/io/file_handler.hpp"
#include <iostream>

int main() {
    try {
        // Load the torrent to get the real tracker URL
        auto content = Torrent::IO::read_file("sample.torrent");
        Torrent::Core::TorrentFile torrent(std::move(content));

        std::string url = torrent.get_tracker_url();
        std::cout << "--- Network Test ---" << std::endl;
        std::cout << "Target URL: " << url << std::endl;

        // 1. Test URL Parsing
        auto addr = Torrent::Network::parse_tracker_url(url);
        std::cout << "1. Parsed Host: " << addr.host << std::endl;
        std::cout << "2. Parsed Port: " << addr.port << std::endl;

        // 2. Test DNS Resolution
        std::cout << "3. Resolving IP..." << std::endl;
        std::string ip = Torrent::Network::resolve_host(addr.host);
        std::cout << "SUCCESS: Resolved to " << ip << std::endl;

    } catch (const std::exception& e) {
        std::cerr << "NETWORK TEST FAILED: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}