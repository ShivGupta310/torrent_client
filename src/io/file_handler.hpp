#pragma once
#include <string>

namespace Torrent::IO{

    //& --> pass by reference, we have complete control of how to access objects.
    std::string read_file(const std::string& path);

}