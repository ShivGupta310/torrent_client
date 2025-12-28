#include "../src/core/bencode.hpp"
#include "../src/io/file_handler.hpp"
#include <iostream>
#include <variant>
#include <string>

// Helper to indent the output
void print_indent(int level) {
    for (int i = 0; i < level; ++i) std::cout << "  ";
}

// The recursive printer
void pretty_print(const Torrent::Core::BValue& val, int level = 0) {
    std::visit([level](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        
        if constexpr (std::is_same_v<T, long long>) {
            std::cout << arg << " (int)\n";
        } 
      else if constexpr (std::is_same_v<T, std::string>) {
    // If it's short, print it. If it's long (like hashes), just print the size.
    if (arg.length() < 100) {
        std::cout << "\"" << arg << "\" (string)\n";
    } else {
        std::cout << "<Binary Data: " << arg.length() << " bytes>\n";
    }
}
        else if constexpr (std::is_same_v<T, Torrent::Core::BList>) {
            std::cout << "List: [\n";
            for (const auto& item : arg) {
                print_indent(level + 1);
                pretty_print(item, level + 1);
            }
            print_indent(level);
            std::cout << "]\n";
        } 
        else if constexpr (std::is_same_v<T, Torrent::Core::BDict>) {
            std::cout << "Dictionary: {\n";
            for (const auto& [key, value] : arg) {
                print_indent(level + 1);
                std::cout << key << " => ";
                pretty_print(value, level + 1);
            }
            print_indent(level);
            std::cout << "}\n";
        }
    }, val.data);
}

int main() {
    try {
        std::string content = Torrent::IO::read_file("sample.torrent");
        std::string_view cursor = content;
        Torrent::Core::BValue result = Torrent::Core::parse_next(cursor);

        std::cout << "--- Bencode Content ---\n";
        pretty_print(result);
        std::cout << "-----------------------\n";

    } catch (const std::exception& e) {
        std::cerr << "CRITICAL ERROR: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}