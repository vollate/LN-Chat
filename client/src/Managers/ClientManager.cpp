#include "ClientManager.hpp"
#include "Room.hpp"
#include "ServerManager.hpp"
#include <memory>

ClientManager::ClientManager() {
    roomList = std::make_shared<QMap<QString, Room>>();
    ip = "127.0.0.1";
}

void ClientManager::createRoom(QString name, QString passWord, ServerManager& serverManager) {
    serverManager.registerRoom(name, passWord);
}

void ClientManager::joinRoom(QString name, QString passWord, QString userName, ServerManager& serverManager) {
    if(serverManager.serverRoomList->contains(name)) {  // Check if the room exists in server
        auto room = (*serverManager.serverRoomList)[name];

        if(room->getPassWord() == passWord) {
            selfPeer = std::make_shared<Peer>(userName, ip);
            this->currentRoom = room;
            room->addPeer(selfPeer);
        } else {
            qDebug() << "Wrong password";
            return;
        }
    } else {
        qDebug() << "Room not found";
        return;
    }
}