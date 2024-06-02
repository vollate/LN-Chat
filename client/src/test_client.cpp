#include "client.hpp"
#include <iostream>
#include <cassert>

int main(int argc, char **argv) {
    ChatClient client("localhost", "11451");

    uint64_t clientId;
    assert(client.RegisterClient("test_user", clientId));
    std::cout << "Client registered with ID: " << clientId << std::endl;

    assert(client.PublishRoom(clientId, "test_room", "password123"));
    std::cout << "Room published successfully" << std::endl;

    std::vector<std::string> peersIp;
    assert(client.GetRoomPeers(clientId, "test_room", "password123", peersIp));
    std::cout << "Peers in room:" << std::endl;
    for (const auto &ip: peersIp) {
        std::cout << ip << std::endl;
    }

    assert(client.HeartBeat(clientId));
    std::cout << "Heartbeat successful" << std::endl;

    return 0;
}
