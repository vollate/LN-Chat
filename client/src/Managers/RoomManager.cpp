#include "RoomManager.hpp"
#include "Room.hpp"
#include <utility>
#include <QMap>
#include <qmap.h>


RoomManager::RoomManager(){
    roomList = new QMap<QString, Room>();
    // TODO: add self peer to room's peer list
}

RoomManager::~RoomManager(){
    delete currentRoom;
    currentRoom = nullptr;
    delete self;
    self = nullptr;
    delete roomList;
    roomList = nullptr;
}

bool RoomManager::registerRoom(QString name, QString passWord){
    
}

bool RoomManager::joinRoom(QString name, QString passWord){
    
}


