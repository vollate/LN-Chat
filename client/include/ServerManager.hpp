#pragma once

#include <QString>
#include <memory>
#include <qlist.h>
#include <qmap.h>
#include <optional>
#include <thread>
#include "Room.hpp"
#include "RpcClient.hpp"

class ServerManager : public QObject{
    Q_OBJECT
public:
    ServerManager(QObject *parent = nullptr);

    void setIp(const QString &ip);
    void setPort(const QString &port);
    void setName(const QString &name);
    void startRpcClient();

    ~ServerManager();

    bool registerClient();

    bool registerRoom(QString room_name, QString room_password);

    void startHeartBeat();

    void stopHeartBeat();

    std::optional<std::list<Peer>> getPeers(const std::string& room_name, const std::string& room_password);
//    Q_INVOKABLE bool registerRoom(QString name, QString passWord);

    std::unique_ptr<QMap<QString, std::shared_ptr<Room>>> serverRoomList;
    constexpr static uint64_t HeartBeat_Interval = 20;
    QString ip;
    QString port;
    uint64_t client_id{};
    std::string client_name;
    std::optional<RpcClient> rpc_client;
    QList<Room> room;
    std::pair<std::atomic_bool, std::thread> heartbeat_daemon;
};
