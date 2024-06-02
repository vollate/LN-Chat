#include "ClientManager.hpp"
#include "Room.hpp"
#include "ServerManager.hpp"
#include <memory>

ClientManager::ClientManager() {
    roomList = std::make_shared<QMap<QString, Room>>();
    ip = "127.0.0.1";
}

void ClientManager::createRoom(QString name, QString passWord, ServerManager &serverManager) {
    serverManager.registerRoom(name.toStdString(), passWord.toStdString());
}

void ClientManager::joinRoom(QString name, QString password, QString username, ServerManager &serverManager) {
    if (auto peers_opt = serverManager.getPeers(name.toStdString(), password.toStdString())) {
        auto peers = peers_opt.value();
        auto room = std::make_shared<Room>(name, password);
        for (auto peer: peers) {
            room->addPeer(std::move(peer));
        }
        roomList->insert(name, *room);
        currentRoom = room;
        selfPeer = std::make_shared<Peer>(username, ip);
    } else {
        qDebug() << "Room not found or wrong password";
        return;
    }
}