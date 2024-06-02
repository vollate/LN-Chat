#include "ServerManager.hpp"

#include <regex>
#include <utility>

namespace {
    std::string urlDecode(const std::string &encoded) {
        std::string decoded;
        char tempChar;
        int hex;

        for (size_t i = 0; i < encoded.length(); ++i) {
            if (encoded[i] == '%' && i + 2 < encoded.length()) {
                std::stringstream ss;
                ss << std::hex << encoded.substr(i + 1, 2);
                ss >> hex;
                tempChar = static_cast<char>(hex);
                decoded += tempChar;
                i += 2;
            } else {
                decoded += encoded[i];
            }
        }
        return decoded;
    }

    std::string extractIPAddress(const std::string &fullAddress) {
        std::string decoded = urlDecode(fullAddress);

        std::regex ipRegex(R"((ipv[46]):(?:%5B)?([0-9a-fA-F:.]+)(?:%5D)?(?::\d+)?$)");
        std::smatch matches;

        if (std::regex_search(decoded, matches, ipRegex) && matches.size() > 2) {
            return matches[2].str();
        }
        return "";
    }
}  // namespace

ServerManager::ServerManager(const std::string &ip, const std::string &port, std::string name)
        : client_name{std::move(name)}, rpc_client(ip, port) {
    while (!registerClient()) {}
    startHeartBeat();
}

ServerManager::~ServerManager() {
    stopHeartBeat();
}

bool ServerManager::registerClient() {
    return rpc_client.RegisterClient(client_name, client_id);
}

std::optional<std::list<Peer>> ServerManager::getPeers(const std::string &room_name, const std::string &room_password) {
    std::vector<PeerInfo> peers;
    if (!rpc_client.GetRoomPeers(client_id, room_name, room_password, peers)) {
        return std::nullopt;
    }
    std::list<Peer> peer_list;
    for (const auto &peer: peers) {
        peer_list.emplace_back(QString::fromStdString(peer.name), QString::fromStdString(extractIPAddress(peer.ip)));
    }
    return peer_list;
}

bool ServerManager::registerRoom(const std::string &room_name, const std::string &room_password) {
    return rpc_client.PublishRoom(client_id, room_name, room_password);
}

void ServerManager::startHeartBeat() {
    auto &[flag, thread] = heartbeat_daemon;
    if (flag) {
        return;
    }
    flag = true;
    thread = std::thread([this] {
        while (heartbeat_daemon.first) {
            rpc_client.HeartBeat(client_id);
            std::this_thread::sleep_for(std::chrono::seconds(HeartBeat_Interval));
        }
    });
}

