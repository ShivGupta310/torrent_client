#pragma once
#include <cstdint>
#include <vector>
#include <string>
#include <array>
#include <netinet/in.h>

namespace Torrent::Network{

    struct TrackerAddress{
        std::string host;
        uint16_t port;
    };

    TrackerAddress parse_tracker_url(std::string_view url);

    std::string resolve_host(const std::string& host);

	//Connection req packet 16 bytes
	struct [[gnu::packed]] ConnectReq{
		uint64_t protocol_id = 0x41727101980 ;//BT defined not me
		uint32_t action = 0; //0 = connect
		uint32_t transaction_id;
	};

    //16 byte response
    struct [[gnu::packed]] ConnectResponse{
        uint32_t action;
        uint32_t transaction_id;
        uint64_t connection_id;
    };

    class TrackerClient{
        public:
            TrackerClient(std::string host, uint16_t port);
            ~TrackerClient();

            //establish udp: get connection id
            uint64_t connect();
        private:
            std::string host;
            uint16_t port;
            int socketfd; //socket file descriptor
            sockaddr_in target_addr;
    };

}
