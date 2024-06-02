#include "ServerManager.hpp"
#include <qmap.h>

ServerManager::ServerManager(){
    serverRoomList = std::make_unique<QMap<QString, Room>>();
}

void ServerManager::registerRoom(QString name, QString passWord){
    Room room(name, passWord);
    serverRoomList->insert(name, room);
}