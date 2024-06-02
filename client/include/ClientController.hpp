#pragma once
#include <QDateTime>
#include "ClientManager.hpp"
#include "Message.hpp"
#include "ServerManager.hpp"
class ClientController : public QObject{
    Q_OBJECT
public:
    explicit ClientController(QObject *parent = nullptr);
    Q_INVOKABLE void createRoom(ClientManager* clientManager, ServerManager* serverManager, QString roomName, QString password);
    Q_INVOKABLE void joinRoom(ClientManager* clientManager, ServerManager* serverManager, QString roomName, QString password);
    Q_INVOKABLE void leaveRoom(ClientManager* clientManager);
    Q_INVOKABLE void sendMessage(ClientManager* clientManager, QString messageText);
//    Q_INVOKABLE void exportMessage(ClientManager& clientManager);

signals:
    void roomJoined(QString roomName);  // 定义信号
    void messageSent(const QString &messageText);
};