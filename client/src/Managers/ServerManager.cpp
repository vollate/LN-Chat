#include "ServerManager.hpp"
#include "utils.hpp"

#include <regex>
#include <utility>

namespace {

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
        peer_list.emplace_back(QString::fromStdString(peer.name),
                               QString::fromStdString(ip_helper::extractIPAddress(peer.ip)));
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

void ServerManager::stopHeartBeat() {
    auto &[flag, thread] = heartbeat_daemon;
    if (!flag) {
        return;
    }
    flag = false;
    if (heartbeat_daemon.second.joinable()) {
        heartbeat_daemon.second.join();
    }
}
