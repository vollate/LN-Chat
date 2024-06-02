#pragma once

#include "Peer.hpp"
#include "Room.hpp"
#include "RpcClient.hpp"
#include <QString>
#include <cstdint>
#include <thread>

class ServerManager {
public:
    ServerManager(const std::string& ip, const std::string& port, std::string name);

    ~ServerManager();

    bool registerClient();

    bool registerRoom(const std::string& room_name, const std::string& room_password);

    void startHeartBeat();

    void stopHeartBeat();

    std::optional<std::list<Peer>> getPeers(const std::string& room_name, const std::string& room_password);

private:
    constexpr static uint64_t HeartBeat_Interval = 20;

    uint64_t client_id{};
    std::string client_name;
    RpcClient rpc_client;
    QList<Room> room;
    std::pair<std::atomic_bool, std::thread> heartbeat_daemon;
};
