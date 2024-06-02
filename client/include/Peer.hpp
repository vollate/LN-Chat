#pragma once

#include <QString>
#include <Room.hpp>

class Peer{

public:
    Peer(QString name, QString ip);
    bool addToRoom(Room room);
    void sendMessage(QString message);

    QString name;
    QString ip;
    Room room;
};