#pragma once
#include <variant>
#include <vector>
#include <string>
#include <map>
#include <string_view>

namespace Torrent::Core{

	//BVal is a type compiler only gonna work w ptrs (vector, map) so it doesnt need to know size, forward declaration 
	struct BValue;

	//Bencode allows byte strings, ints, lists, dictionaries
	using BList = std::vector<BValue>;
	using BDict = std::map<std::string, BValue>;

	struct BValue{
   	 
	  std::variant<long long, std::string, BList, BDict> data;	
    };

	//Function sig
	BValue parse_next(std::string_view& cursor, std::string_view& info_slice);

}
