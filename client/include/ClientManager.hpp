#pragma once

#include "Peer.hpp"
#include "Room.hpp"
#include "ServerManager.hpp"
#include "Room.hpp"

#include <QCoreApplication>
#include <QDebug>
#include <QMap>
#include <QTcpServer>
#include <QTcpSocket>
#include <memory>
#include <qmap.h>

#include <QDebug>
#include <qobject.h>


class ClientManager final : public QObject {

    Q_OBJECT

public:
    ClientManager(quint16 port = Client_Server_Port, QObject *parent = nullptr);

    ~ClientManager() override;

    void createRoom(const QString &name, const QString &password, ServerManager &serverManager);

    void joinRoom(const QString &name, const QString &password, const QString &username, ServerManager &serverManager);

    void leaveRoom();

    void sendMessage(const Message &message);

    void exportMessage();

    void loadMessage();

    void getRoomList();

private:

    void handleNewConnection();

    QString ip;
    std::mutex mutex;

    std::unique_ptr<QTcpServer> tcp_server;
    std::shared_ptr<Room> currentRoom;
    std::shared_ptr<QMap<QString, Room>> roomList;
};
