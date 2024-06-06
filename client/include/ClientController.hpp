#pragma once
#include "ClientManager.hpp"
#include "Message.hpp"
#include "ServerManager.hpp"
#include <QDateTime>

class ClientController : public QObject {
    Q_OBJECT
public:
    explicit ClientController(QObject* parent = nullptr);
    Q_INVOKABLE void connectToServer(ServerManager* serverManager, QString ip, QString port);
    Q_INVOKABLE void createRoom(ClientManager* clientManager, ServerManager* serverManager, QString roomName, QString password);
    Q_INVOKABLE void joinRoom(ClientManager* clientManager, ServerManager* serverManager, QString roomName, QString password);
    Q_INVOKABLE void leaveRoom(ClientManager* clientManager);
    Q_INVOKABLE void sendMessage(ClientManager* clientManager, QString messageText);
    Q_INVOKABLE QVariantList getCurrentRoomMsgs(ClientManager* clientManager);
    Q_INVOKABLE void exportMessage(ClientManager* clientManager);
    Q_INVOKABLE void setRoom(ClientManager* clientManager, QString roomName);
    Q_INVOKABLE void loadMessage(ClientManager* clientManager, const QString& roomName);

    QString toReadableTime(QString timestamp);

signals:
    void roomJoined(QString roomName);  // 定义信号
};