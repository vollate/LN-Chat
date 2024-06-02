#include "ClientManager.hpp"
#include "ServerManager.hpp"

ClientManager::ClientManager(){
    roomList = std::make_shared<QMap<QString, Room>>();
}

void ClientManager::createRoom(QString name, QString passWord, ServerManager& serverManager){
    serverManager.registerRoom(name, passWord);
}

void ClientManager::joinRoom(QString name, QString passWord, QString userName, ServerManager& serverManager){
    if(serverManager.serverRoomList->contains(name)){
        Room& room = (*serverManager.serverRoomList)[name];

        if (room.getPassWord() == passWord){
            selfPeer = std::make_shared<Peer>(userName, "127.0.0.1");
            
        }
    }else{
        qDebug() << "Room not found";
        return;
    }
}