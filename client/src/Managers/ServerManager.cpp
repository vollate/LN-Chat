#include "ServerManager.hpp"
#include "utils.hpp"

#include <regex>
#include <utility>

namespace {}  // namespace

ServerManager::ServerManager(QObject* parent) : client_name{}, rpc_client{}, QObject(parent) {}

ServerManager::~ServerManager() {
    stopHeartBeat();
}

bool ServerManager::registerClient() {
    return rpc_client.value().RegisterClient(client_name, client_id);
}

std::optional<std::list<Peer>> ServerManager::getPeers(const std::string& room_name, const std::string& room_password,
                                                       std::string& self_ip) {
    std::vector<PeerInfo> peers;
    std::string ip;
    if(!rpc_client.value().GetRoomPeers(client_id, room_name, room_password, peers, ip)) {
        return std::nullopt;
    }
    std::list<Peer> peer_list;
    for(const auto& peer : peers) {
        peer_list.emplace_back(QString::fromStdString(peer.name), QString::fromStdString(ip_helper::extractIPAddress(peer.ip)));
    }
    ip = ip_helper::extractIPAddress(ip);
    return peer_list;
}

auto ServerManager::registerRoom(const QString& room_name, const QString& room_password) -> bool {
    return rpc_client.value().PublishRoom(client_id, room_name.toStdString(), room_password.toStdString());
}

void ServerManager::startHeartBeat() {
    auto& [flag, thread] = heartbeat_daemon;
    if(flag) {
        return;
    }
    flag = true;
    thread = std::thread([this] {
        while(heartbeat_daemon.first) {
            rpc_client.value().HeartBeat(client_id);
            std::this_thread::sleep_for(std::chrono::seconds(HeartBeat_Interval));
        }
    });
}

void ServerManager::stopHeartBeat() {
    auto& [flag, thread] = heartbeat_daemon;
    if(!flag) {
        return;
    }
    flag = false;
    if(heartbeat_daemon.second.joinable()) {
        heartbeat_daemon.second.join();
    }
}

void ServerManager::setIp(const QString& target_ip) {
    this->ip = target_ip;
}

void ServerManager::setPort(const QString& target_port) {
    this->port = target_port;
}

void ServerManager::setName(const QString& name) {
    client_name = name.toStdString();
}

void ServerManager::startRpcClient() {
    rpc_client = std::move(RpcClient(ip.toStdString(), port.toStdString()));
    while(!registerClient()) {}
    startHeartBeat();
}

std::vector<QString> ServerManager::getRoomList() {
    std::vector<std::string> rooms;
    std::vector<QString> res;
    if(rpc_client.value().GetAllRooms(client_id, rooms)) {
        for(const auto& room_name : rooms) {
            res.push_back(QString::fromStdString(room_name));
        }
    }
    return res;
}
