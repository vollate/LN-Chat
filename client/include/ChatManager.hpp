#pragma once

#include <QString>
#include "Peer.hpp"
#include "Room.hpp"
#include "RpcClient.hpp"

class ChatManager {
public:
    ChatManager(const std::string &ip, const std::string &port, std::string name);

    bool registerClient();

    std::optional<QList<Peer>> getPeers(const std::string &room_name, const std::string &room_password);

private:
    uint64_t client_id{};
    std::string client_name;
    RpcClient rpc_client;
    QList<Room> room;

};