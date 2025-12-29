#pragma once
#include "bencode.hpp"
#include <string>
#include <vector>
#include <array>

namespace Torrent::Core{
    class TorrentFile{

        public:
            //raw file data
            explicit TorrentFile(std::string raw_bytes);

            
            std::array<unsigned char, 20> get_info_hash() const {return info_hash;}
            std::string get_info_hash_hex() const;

            std::string get_tracker_url() const;
            long long get_total_size() const;

        private:
            std::string raw_data;
            //Fingerprint (160 bit sha1)
            std::array<unsigned char, 20> info_hash;
            BValue root; //fully parsed tree from .torrent

            //helper
            void calc_info_hash(std::string_view info_slice);
    };
}