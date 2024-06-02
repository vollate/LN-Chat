#include "ClientManager.hpp"
#include "Message.hpp"
#include "ServerManager.hpp"

int main() {
    ClientManager clientManager("a");
    ServerManager serverManager;
    clientManager.createRoom("room1", "1234", serverManager);
    clientManager.joinRoom("room1", "1234", serverManager);

    ClientManager clientManager2("b");
    clientManager2.joinRoom("room1", "123", serverManager);

    ClientManager clientManager3("c");
    clientManager3.joinRoom("room1", "1234", serverManager);

    for (auto peer : clientManager.currentRoom->peers) {
        qDebug() << peer.name;
    }

    clientManager.sendMessage("Hello");
    clientManager2.sendMessage("Hi");
    clientManager3.sendMessage("Hey");

    qDebug() << "Messages in room1:";
    for (auto& message : clientManager.currentRoom->messages) {
        qDebug() << message.sender << message.time << message.text;
    }

    return 0;
}