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

    std::string TorrentFile::get_tracker_url() const {
        const auto& dict = std::get<BDict>(root.data);    
        if (dict.count("announce")) {
            return std::get<std::string>(dict.at("announce").data);
        }
        else{
            return "";
        }
    }

    long long TorrentFile::get_total_size() const {
        const auto& root_dict = std::get<BDict>(root.data);
        const auto& info = std::get<BDict>(root_dict.at("info").data);

        // Case 1: one file
        if (info.count("length")) {
            return std::get<long long>(info.at("length").data);
        }

        //Case 2: many files

        if (info.count("files")) {
            long long total = 0;
            const auto& files = std::get<BList>(info.at("files").data);

            for (const auto& file_val : files) {
                const auto& file_dict = std::get<BDict>(file_val.data);
                total += std::get<long long>(file_dict.at("length").data);
            }

            return total;
        }

        return 0;
    }

    
}