#pragma once

#include "Peer.hpp"
#include "Room.hpp"
#include "ServerManager.hpp"

#include <QCoreApplication>
#include <QDebug>
#include <QMap>
#include <QTcpServer>
#include <QTcpSocket>
#include <memory>

class ClientManager : public QObject {

Q_OBJECT

public:
    ClientManager(QObject* parent = nullptr);
    ~ClientManager();

    void createRoom(QString name, QString passWord, ServerManager &serverManager);

    void joinRoom(QString name, QString password, QString username, ServerManager &serverManager);

    void leaveRoom();

    void sendMessage();

    void exportMessage();

    void loadMessage();

    void getRoomList();

private:

    void handleNewConnection();

    QString ip;

    std::unique_ptr<QTcpServer> tcp_server;
    std::shared_ptr<Room> currentRoom;
    std::shared_ptr<Peer> selfPeer;
    std::shared_ptr<QMap<QString, Room>> roomList;
};
