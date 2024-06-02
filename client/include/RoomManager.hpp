#pragma once
#include "Room.hpp"
#include "Peer.hpp"

#include <QMap>
#include <qmap.h>

#include <QDebug>


class RoomManager{

public:
    RoomManager();
    ~RoomManager();
    bool registerRoom(QString name, QString passWord);
    bool joinRoom(QString name, QString passWord);
    void getRoomList();

private:
    Room* currentRoom;
    Peer* self;
    QMap<QString, Room>* roomList;
};