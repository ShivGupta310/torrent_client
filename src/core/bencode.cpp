#include "bencode.hpp"
#include <stdexcept>
#include <charconv>

namespace Torrent::Core{

    BValue parse_next(std::string_view& cursor){

        if(cursor.empty()){
            throw std::runtime_error("Empty cursor/buffer");
        }

        char lead = cursor[0];

        switch(lead){
            //int
            case 'i': {
                cursor.remove_prefix(1);
                //find end 'e'
                size_t end = cursor.find('e');

                if (end == std::string_view::npos){
                    throw std::runtime_error("Invalid bencode int: missing 'e'");
                }

                //number substr
                std::string_view num_str = cursor.substr(0, end);

                long long val = 0;
                auto [ptr, ec] = std::from_chars(num_str.data(), num_str.data() + num_str.size(), val);
                if (ec != std::errc()) {
                    throw std::runtime_error("Invalid bencode int format");
                }

                //adv cursor
                cursor.remove_prefix(end + 1);

                return BValue{val};
            }
            
            //list
            case 'l': {
                cursor.remove_prefix(1);
                BList list;

                while(!cursor.empty() && cursor[0] != 'e'){
                    BValue item = parse_next(cursor);
                    list.push_back(item);
                }

                if (cursor.empty()){
                    throw std::runtime_error("Invalid bencode list: missing 'e'");
                }

                //advance past 'e'
                cursor.remove_prefix(1);
                return BValue{list};
            }

            //dict
            case 'd': {
                cursor.remove_prefix(1);
                BDict dict;

                while(!cursor.empty() && cursor[0] != 'e'){

                    BValue key_val = parse_next(cursor);
                    std::string key = std::get<std::string>(key_val.data);

                    BValue val = parse_next(cursor);
                    
                    dict[key] = val;

                }
                
                if (cursor.empty()){
                    throw std::runtime_error("Invalid bencode dictionary: missing 'e'");
                }

                cursor.remove_prefix(1);
                return BValue{dict};
            }
            
            default: {
                if (std::isdigit(lead)){

                    size_t colon = cursor.find(':');
                    if (colon == std::string_view::npos){
                        throw std::runtime_error("Invalid bencode string: missing ':'");
                    }

                    std::string_view len_str = cursor.substr(0, colon);
                    size_t len = 0;

                    auto [ptr, ec] = std::from_chars(len_str.data(), len_str.data() + len_str.size(), len);
                    
                    if (ec == std::errc()) {
                        //advance cursor past length and colon

                        //overflow check 
                        if (cursor.size() < colon + 1 + len){
                            throw std::runtime_error("Invalid bencode string: length exceeds buffer size");
                        }
                        cursor.remove_prefix(colon + 1);
                        
                       std::string bytes = std::string(cursor.substr(0, len));
                        //advance the cursor and return
                        cursor.remove_prefix(len);
                        return BValue{ bytes};
                    
                    } else if (ec == std::errc::invalid_argument) {
                        throw std::runtime_error("Invalid string length: not a number");
                    } else if (ec == std::errc::result_out_of_range) {
                        throw std::runtime_error("String length out of range");
                    }
                
                }
                else{
                    throw std::runtime_error("Invalid bencode lead character");
                }
            }
        }
        throw std::runtime_error("Invalid bencode character encountered");
    }

}