#include <QJsonDocument>
#include <memory>

#include "ClientManager.hpp"
#include "Constant.hpp"
#include "Message.hpp"
#include "Peer.hpp"
#include "Room.hpp"
#include "ServerManager.hpp"
#include "utils.hpp"

ClientManager::ClientManager(quint16 port, QObject* parent)
    : QObject{ parent }, tcp_server{ this }, roomList{ std::make_shared<QMap<QString, std::shared_ptr<Room>>>() } {
    if(!tcp_server.listen(QHostAddress::Any, port)) {
        qCritical() << "Server could not start:" << tcp_server.errorString();
    } else {
        qDebug() << "Server started on port " << port << '\n';
        connect(&tcp_server, &QTcpServer::newConnection, this, &ClientManager::handleNewConnection);
    }
}

QString ClientManager::getUserName() const {
    return userName;
}

void ClientManager::createRoom(const QString& name, const QString& password, ServerManager* serverManager) {
    serverManager->registerRoom(name, password);
}

auto ClientManager::joinRoom(const QString& name, const QString& password, ServerManager* serverManager) -> bool {
    std::string ip;
    if(auto peers_opt = serverManager->getPeers(name.toStdString(), password.toStdString(), ip)) {
        this->ip = QString::fromStdString(ip);
        auto peers = peers_opt.value();
        auto room = std::make_shared<Room>(name, password);
        std::lock_guard guard{ mutex };
        currentRoom = room;
        roomList->insert(name, room);
        for(const auto& peer : peers) {
            qDebug() << "add Peer: " << peer.ip << " " << peer.name;
            sendJoinSignal(peer);
            room->addPeer(peer);
        }
        return true;
    }
    qDebug() << "Room not found or wrong password";
    return false;
}

void ClientManager::setUserName(const QString& name) {
    userName = name;
}

ClientManager::~ClientManager() {
    tcp_server.close();
}

bool ClientManager::sendMessage(const Message& message) {
    if(currentRoom) {
        auto msg_json = json_helper::message2Json(message, currentRoom->getName());
        QJsonDocument doc(msg_json);
        auto bytes = doc.toJson();
        for(const auto& peer : currentRoom->getPeers()) {
            qDebug() << "ip: " << peer.ip << " name " << peer.name;
            auto socket = peer.getSocket();
            if(socket != nullptr && socket->isOpen()) {
                socket->write(bytes);
                if(socket->waitForBytesWritten(Max_Send_Wait_Time)) {
                    qDebug() << "Message sent to " << peer.name << " successfully";
                } else {
                    qDebug() << "Failed to send message to " << peer.name;
                }
            }
        }
        return true;
    }
    return false;
}

void ClientManager::handleNewConnection() {
    QTcpSocket* clientSocket = tcp_server.nextPendingConnection();
    if(clientSocket != nullptr) {
        connect(clientSocket, &QTcpSocket::readyRead, this, [this, clientSocket]() {
            QByteArray data = clientSocket->readAll();
            qDebug() << "Received data:" << data;
            auto json_data = QJsonDocument::fromJson(data).object();
            auto room_name = json_data["roomName"].toString();
            if(!json_data["hello"].toString().isEmpty()) {
                auto peer = json_helper::json2Peer(json_data);
                auto& target_room = roomList->find(room_name).value();
                target_room->addPeer(peer);
                qDebug() << "Peer connected: " << peer.name;
            } else {
                auto msg = json_helper::json2Message(json_data);
                std::lock_guard guard{ mutex };
                auto& target_room = roomList->find(msg.second).value();
                qDebug() << "Message received: " << msg.first.text;
                QString rcvMsg = msg.first.sender + "\n" + msg.first.text;
                emit messageSent(rcvMsg);
                target_room->addMessage(msg.first);
            }
        });
    }
}

void ClientManager::leaveRoom() {
    if(currentRoom) {
        currentRoom->removePeer(userName);
        currentRoom = nullptr;
    } else {
        qDebug() << "You are not in a room";
    }
}

void ClientManager::exportMessage() {
    // TODO
}

void ClientManager::loadMessage() {
    // TODO
}

void ClientManager::sendJoinSignal(const Peer& peer) const {
    QJsonObject json;
    json["hello"] = "514";
    json["name"] = userName;
    json["roomName"] = currentRoom->getName();
    json["ip"] = ip;
    QJsonDocument doc(json);
    auto bytes = doc.toJson();
    auto socket = peer.getSocket();
    if(socket != nullptr && socket->isOpen()) {
        socket->write(bytes);
        if(socket->waitForBytesWritten(Max_Send_Wait_Time)) {
            qDebug() << "Join signal sent to " << peer.name;
        } else {
            qDebug() << "Failed to send join signal to " << peer.name;
        }
    }
}
