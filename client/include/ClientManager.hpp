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
#include <mutex>
#include <qobjectdefs.h>


class ClientManager final : public QObject {

Q_OBJECT

public:
    ClientManager(quint16 port = Client_Server_Port, QObject *parent = nullptr);

    ~ClientManager() override;

    void createRoom(const QString &name, const QString &password, ServerManager *serverManager);

    bool joinRoom(const QString &name, const QString &password, ServerManager *serverManager);

    void leaveRoom();

    bool sendMessage(const Message &message);

    Q_INVOKABLE void setUserName(const QString &name);

    void exportMessage();

    void loadMessage();

    void getRoomList();

    Q_INVOKABLE void handleNewConnection();

    QString ip;
    std::mutex mutex;

    QString userName;

    std::unique_ptr<QTcpServer> tcp_server;
    std::shared_ptr<Room> currentRoom;
    std::shared_ptr<QMap<QString, Room>> roomList;


signals:
    void messageSent(const QString &messageText);
};
