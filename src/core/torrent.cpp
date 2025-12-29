#include "torrent.hpp"
#include <openssl/sha.h>
#include <iomanip>
#include <sstream>

namespace Torrent::Core{

    //Constructor 
    TorrentFile::TorrentFile(std::string raw_bytes) : raw_data(std::move(raw_bytes)) //move ownership of str from caller to this class
    {

        std::string_view info_slice;
        std::string_view cursor = this->raw_data;

        this->root = parse_next(cursor, info_slice);

        if (info_slice.empty()){
            throw std::runtime_error("Unable to parse/missing info dict in bencode");
        }

        SHA1(reinterpret_cast<const unsigned char*>(info_slice.data()), 
             info_slice.size(), 
             this->info_hash.data());
    }
    
    //hex fn
    std::string TorrentFile::get_info_hash_hex() const {
        std::stringstream ss;
        ss << std::hex << std::setfill('0');
        for (unsigned char byte : info_hash) {
            ss << std::setw(2) << static_cast<int>(byte);
        }
        return ss.str();
    }
}