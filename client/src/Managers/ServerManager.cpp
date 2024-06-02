#include "ServerManager.hpp"
#include "utils.hpp"

#include <regex>
#include <utility>

namespace {

}  // namespace

ServerManager::ServerManager(
        QObject *parent)
        : client_name{}, rpc_client{}, QObject(parent) {

}

ServerManager::~ServerManager() {
    stopHeartBeat();
}

bool ServerManager::registerClient() {
    return rpc_client.value().RegisterClient(client_name, client_id);
}

std::optional<std::list<Peer>> ServerManager::getPeers(const std::string &room_name, const std::string &room_password) {
    std::vector<PeerInfo> peers;
    if (!rpc_client.value().GetRoomPeers(client_id, room_name, room_password, peers)) {
        return std::nullopt;
    }
    std::list<Peer> peer_list;
    for (const auto &peer: peers) {
        peer_list.emplace_back(QString::fromStdString(peer.name),
                               QString::fromStdString(ip_helper::extractIPAddress(peer.ip)));
    }
    return peer_list;
}

bool ServerManager::registerRoom(QString room_name, QString room_password) {
    return rpc_client.value().PublishRoom(client_id, room_name.toStdString(), room_password.toStdString());
}

void ServerManager::startHeartBeat() {
    auto &[flag, thread] = heartbeat_daemon;
    if (flag) {
        return;
    }
    flag = true;
    thread = std::thread([this] {
        while (heartbeat_daemon.first) {
            rpc_client.value().HeartBeat(client_id);
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

void ServerManager::setIp(const QString &ip) {
    this->ip = ip;
}

void ServerManager::setPort(const QString &port) {
    this->port = port;
}

void ServerManager::setName(const QString &name) {
    client_name = name.toStdString();
}

void ServerManager::startRpcClient() {
    rpc_client= std::move(RpcClient(ip.toStdString(), port.toStdString()));
    while (!registerClient()) {}
    startHeartBeat();
}
