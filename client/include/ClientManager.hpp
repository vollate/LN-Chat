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
#include <qchar.h>
#include <qjsonarray.h>
#include <qmap.h>

#include <QDebug>
#include <mutex>
#include <qobject.h>
#include <qobjectdefs.h>

#include <random>

class ClientManager final : public QObject {

    Q_OBJECT

public:
    ClientManager(quint16 port = Client_Server_Port, QObject* parent = nullptr);

    ~ClientManager() override;

    static void createRoom(const QString& name, const QString& password, ServerManager* serverManager);

    bool joinRoom(const QString& name, const QString& password, ServerManager* serverManager);

    void leaveRoom();

    bool sendMessage(const Message& message);

    Q_INVOKABLE void setUserName(const QString& name);
    Q_INVOKABLE QString getUserName() const;

    void exportMessage(const QList<Message> &messages , const QString &path , const QString &name);

    void loadMessage(const QString &path, const QString &name);

    void getRoomList();

    Q_INVOKABLE void handleNewConnection();

    QString ip;
    QString serverIp;
    std::mutex mutex;

    QString userName;

    QTcpServer tcp_server;
    std::shared_ptr<Room> currentRoom;
    std::shared_ptr<QMap<QString, Room>> roomList;

signals:
    void messageSent(const QString &messageText);
    void fileError(const QString &roomName);
    void fileLoaded(const QVariantList &messageList, const QString &roomName);
};
