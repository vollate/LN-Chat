#pragma once

#include "Room.hpp"
#include "RpcClient.hpp"
#include <QString>
#include <memory>
#include <optional>
#include <qlist.h>
#include <qmap.h>
#include <thread>

class ServerManager final : public QObject {
    Q_OBJECT
public:
    explicit ServerManager(QObject* parent = nullptr);

    ~ServerManager() override;

    void setIp(const QString& target_ip);

    void setPort(const QString& target_port);

    void setName(const QString& name);

    void startRpcClient();

    auto registerClient() -> bool;

    auto registerRoom(const QString& room_name, const QString& room_password) -> bool;

    void startHeartBeat();

    void stopHeartBeat();

    auto getRoomList() -> std::vector<QString>;

    auto getPeers(const std::string& room_name, const std::string& room_password, std::string& self_ip)
        -> std::optional<std::list<Peer>>;

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
