#pragma once
#include "Message.hpp"
#include "ServerManager.hpp"
#include "Room.hpp"

#include <QMap>
#include <memory>
#include <qmap.h>

#include <QDebug>
#include <qobject.h>
#include <QString>
#include <QDateTime>



class ClientManager : public QObject{
    Q_OBJECT

public:
    explicit ClientManager(QObject *parent = nullptr);
    ~ClientManager();
    Q_INVOKABLE void createRoom(QString name, QString passWord, ServerManager* serverManager);
    Q_INVOKABLE void joinRoom(QString name, QString password, ServerManager* serverManager);
    Q_INVOKABLE void leaveRoom();
    Q_INVOKABLE void sendMessage(QString messageText);
    Q_INVOKABLE void setUserName(QString name);
//    Q_INVOKABLE void exportMessage();
//    Q_INVOKABLE void loadMessage();
//    Q_INVOKABLE void getRoomList();

    QString ip;
    QString userName;
    std::shared_ptr<Room> currentRoom;
    std::shared_ptr<QMap<QString, Room>> roomList;
};