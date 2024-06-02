#pragma once
#include "Room.hpp"
#include "Peer.hpp"
#include "ServerManager.hpp"

#include <QMap>
#include <memory>
#include <qmap.h>

#include <QDebug>
#include <qobject.h>


class ClientManager : QObject{

    Q_OBJECT

public:
    ClientManager();
    ~ClientManager();
    void createRoom(QString name, QString passWord, ServerManager& serverManager);
    void joinRoom(QString name, QString password, QString username, ServerManager& serverManager);
    void leaveRoom();
    void sendMessage();
    void exportMessage();
    void loadMessage();
    void getRoomList();

private:
    QString ip;
    std::shared_ptr<Room> currentRoom;
    std::shared_ptr<Peer> selfPeer;
    std::shared_ptr<QMap<QString, Room>> roomList;
};