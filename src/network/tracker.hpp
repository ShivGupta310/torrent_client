#pragma once
#include <cstdint>
#include <vector>
#include <string>
#include <array>

namespace Torrent::Network{

    struct TrackerAddress{
        std::string host;
        uint16_t port;
    };

    TrackerAddress parse_tracker_url(std::string_view url);

    std::string resolve_host(const std::string& host);

	//Connection req packet 16 bytes
	struct ConnectReq{
		uint64_t protocol_id = 0x41727101980 ;//BT defined not me
		uint32_t action = 0; //0 = connect
		uint32_t transaction_id;
	};

    //16 byte response
    struct ConnectResponse{
        uint32_t action;
        uint32_t transaction_id;
        uint64_t connection_id;
    };

}
