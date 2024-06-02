#include "RpcClient.hpp"
#include <iostream>
#include <cassert>

int main(int argc, char **argv) {
    RpcClient client("localhost", "11451");

    uint64_t clientId;
    assert(client.RegisterClient("test_user", clientId));
    std::cout << "Client registered with ID: " << clientId << std::endl;

    assert(client.PublishRoom(clientId, "test_room", "password123"));
    std::cout << "Room published successfully" << std::endl;

    std::vector<PeerInfo> peers;
    assert(client.GetRoomPeers(clientId, "test_room", "password123", peers));
    std::cout << "Peers in room:" << std::endl;
    for (const auto &peer: peers) {
        std::cout << "IP: " << peer.ip << ", Name: " << peer.name << std::endl;
    }

    assert(client.HeartBeat(clientId));
    std::cout << "Heartbeat successful" << std::endl;

    return 0;
}
