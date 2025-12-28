#include "file_handler.hpp"
#include <fstream>
#include <stdexcept>
#include <string>

namespace Torrent::IO{

    std::string read_file(const std::string& path) {
    // std::ios::binary -> raw bytes
    // std::ios::ate    -> "At The End" (starts the cursor at the end of the file)

    std::ifstream file(path.data(), std::ios::binary | std::ios::ate);

    if (!file.is_open()) {
        throw std::runtime_error("Could not open file");
    }

    //get filesize
    std::streamsize size = file.tellg();

    std::string buffer(size, '\0');

    file.seekg(0, std::ios::beg);

    if (file.read(buffer.data(), size)) {
        return buffer;
    }

    throw std::runtime_error("Failed to read file data");
    }

}
