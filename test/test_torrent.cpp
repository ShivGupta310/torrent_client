#include "../src/core/torrent.hpp"
#include "../src/io/file_handler.hpp"
#include <iostream>

int main() {
    try {
        std::string content = Torrent::IO::read_file("sample.torrent");
        Torrent::Core::TorrentFile torrent(std::move(content));

        std::cout << "--- Torrent Metadata ---" << std::endl;
        std::cout << "Info Hash (Hex): " << torrent.get_info_hash_hex() << std::endl;
        std::cout << "Tracker URL: " << torrent.get_tracker_url() << std::endl;
        std::cout << "Total Size:  " << torrent.get_total_size() << " bytes" << std::endl;

    } catch (const std::exception& e) {
        std::cerr << "Test failed: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}