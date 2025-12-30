#include "tracker.hpp"
#include <stdexcept>
#include <charconv>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <iostream> // temp debugging

namespace Torrent::Network{

    TrackerAddress parse_tracker_url(std::string_view url){

        size_t protocol_end = url.find("://");
        if (protocol_end == std::string_view::npos){
            throw std::runtime_error("Invalid tracker URL: missing ://");
        }
        url.remove_prefix(protocol_end + 3);

        size_t colon_pos = url.find(':');
        if (colon_pos == std::string_view::npos) {
            throw std::runtime_error("Invalid tracker URL: missing port");
        }

        std::string host = std::string(url.substr(0, colon_pos));
        url.remove_prefix(colon_pos + 1);

        size_t slash_pos = url.find('/');
        std::string_view port_str = (slash_pos == std::string_view::npos) 
                                    ? url 
                                    : url.substr(0, slash_pos);
        
        uint16_t port = 0;

        auto [ptr, ec] = std::from_chars(port_str.data(), port_str.data() + port_str.size(), port);
        
        if (ec != std::errc()) {
            throw std::runtime_error("Invalid tracker port number");
        }

        return TrackerAddress{ host, port };
    }

    std::string resolve_host(const std::string& host) {
        addrinfo hints{};
        addrinfo* result = nullptr;

        hints.ai_family = AF_INET;       // IPv4
        hints.ai_socktype = SOCK_DGRAM; // UDP

        // DNS call
        int s = getaddrinfo(host.c_str(), nullptr, &hints, &result);
        if (s != 0) {
            throw std::runtime_error("DNS Resolution failed for " + host + ": " + gai_strerror(s));
        }

        // Convert the binary IP address to readable string
        char ip_str[INET_ADDRSTRLEN];
        sockaddr_in* ipv4 = reinterpret_cast<sockaddr_in*>(result->ai_addr);
        inet_ntop(AF_INET, &(ipv4->sin_addr), ip_str, INET_ADDRSTRLEN);

        // Free the memory allocated by getaddrinfo
        freeaddrinfo(result);

        return std::string(ip_str);
    }
}