#include "tracker.hpp"
#include <stdexcept>
#include <charconv>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <iostream> // temp debugging
#include <random>
#include <unistd.h> //close

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

    TrackerClient::TrackerClient(std::string h, uint16_t p) : host(std::move(h)), port(p){

        //create socket AF_INET = IPv4, SOCK_DGRAM = UDP
        socketfd = socket(AF_INET, SOCK_DGRAM, 0);
        if (socketfd < 0) throw std::runtime_error("Failed to create socket");

        target_addr.sin_family = AF_INET;
        target_addr.sin_port = htons(port); //flip port little to big endian

        std::string ip = resolve_host(host);
        inet_pton(AF_INET, ip.c_str(), &target_addr.sin_addr);

        //set timeout
        struct timeval tv;
        tv.tv_sec = 30; // 30 seconds
        tv.tv_usec = 0;
        setsockopt(socketfd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));
    }

    TrackerClient::~TrackerClient() {
        if (socketfd >= 0) close(socketfd);
    }

    uint64_t TrackerClient::connect(){
        ConnectReq req;
        
        //DEBUG
        if (sizeof(ConnectReq) != 16) {
            throw std::runtime_error("Struct padding error! Size is " + std::to_string(sizeof(ConnectReq)));
        }

        // Random transaction ID to id response
        std::random_device rd;
        req.transaction_id = rd();

        // Flip EVERYTHING to big endian (network byte order for some reason)
        ConnectReq network_req;
        network_req.protocol_id = htobe64(req.protocol_id);
        network_req.action = htonl(0); // 0 is connect
        network_req.transaction_id = htonl(req.transaction_id);

        std::cout << "[DEBUG] Raw Packet Hex: ";
        unsigned char* p = (unsigned char*)&network_req;
        for(int i = 0; i < 16; i++) {
            printf("%02x ", p[i]);
        }
        std::cout << std::endl;

        //send packet
        sendto(socketfd, &network_req, sizeof(network_req), 0,
           (struct sockaddr*)&target_addr, sizeof(target_addr));

        //Receive response from socket
        ConnectResponse res;
        socklen_t addr_len = sizeof(target_addr);
        ssize_t rec = recvfrom(socketfd, &res, sizeof(res), 0,
                            (struct sockaddr*)&target_addr, &addr_len);
        
        if (rec < 16) throw std::runtime_error("Tracker connection timeout or invalid response");

        //Flip back to little endian
        uint32_t action = ntohl(res.action);
        uint32_t trans_id = ntohl(res.transaction_id);
        uint64_t conn_id = be64toh(res.connection_id);

        //Ensure tracker replying to us --> does random id match
        if (trans_id != req.transaction_id) throw std::runtime_error("Transaction ID mismatch!");

        return conn_id;
    }
}