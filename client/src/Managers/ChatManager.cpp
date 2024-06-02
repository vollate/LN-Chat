#include "ChatManager.hpp"

#include <utility>

ChatManager::ChatManager(const std::string &ip, const std::string &port, std::string name) : client_name{
        std::move(name)},
                                                                                             rpc_client(ip,
                                                                                                        port) {
}

bool ChatManager::registerClient() {
    return rpc_client.RegisterClient(client_name, client_id);
}

std::optional<QList<Peer>> ChatManager::getPeers(const std::string &room_name, const std::string &room_password) {
    std::vector<PeerInfo> peers;
    rpc_client.GetRoomPeers(client_id, room_name, room_password, peers);
    QList<Peer> peer_list;
    for (const auto &peer: peers) {

    }
    return std::nullopt;
}
