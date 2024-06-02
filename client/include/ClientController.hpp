#pragma once

#include "ClientManager.hpp"
#include "ServerManager.hpp"
class ClientController : public QObject{
    Q_OBJECT
public:
    explicit ClientController(QObject *parent = nullptr);
    Q_INVOKABLE void createRoom(ClientManager& clientManager, ServerManager& serverManager, QString roomName, QString password);
    Q_INVOKABLE void joinRoom(ClientManager& clientManager, ServerManager& serverManager, QString roomName, QString password);
    Q_INVOKABLE void leaveRoom(ClientManager& clientManager);
    Q_INVOKABLE void sendMessage(ClientManager& clientManager);
//    Q_INVOKABLE void exportMessage(ClientManager& clientManager);
};