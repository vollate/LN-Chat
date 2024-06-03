#include <memory>
#include <QJsonDocument>

#include "ClientManager.hpp"
#include "Message.hpp"
#include "Peer.hpp"
#include "Room.hpp"
#include "ServerManager.hpp"
#include "utils.hpp"

ClientManager::ClientManager(quint16 port, QObject *parent)
        : QObject{parent}, tcp_server{std::make_unique<QTcpServer>(this)},
          roomList{std::make_shared<QMap<QString, Room>>()},
          ip{"127.0.0.1"} {
    if (!tcp_server->listen(QHostAddress::Any, port)) {
        qCritical() << "Server could not start:" << tcp_server->errorString();
    } else {
        qDebug() << "Server started on port " << port << '\n';
        connect(tcp_server.get(), &QTcpServer::newConnection, this, &ClientManager::handleNewConnection);
    }
}

QString ClientManager::getUserName() {
    return userName;
}

void ClientManager::createRoom(const QString &name, const QString &password, ServerManager *serverManager) {
    serverManager->registerRoom(name, password);
}

bool ClientManager::joinRoom(const QString &name, const QString &password, ServerManager *serverManager) {
    if (auto peers_opt = serverManager->getPeers(name.toStdString(), password.toStdString())) {
        auto peers = peers_opt.value();
        auto room = std::make_shared<Room>(name, password);
        for (auto peer: peers) {
            room->addPeer(std::move(peer));
        }
        std::lock_guard guard{mutex};
        roomList->insert(name, *room);
        currentRoom = room;
        return true;
    }
    qDebug() << "Room not found or wrong password";
    return false;

}

void ClientManager::setUserName(const QString &name) {
    userName = name;
}

ClientManager::~ClientManager() {
    tcp_server->close();
}

bool ClientManager::sendMessage(const Message &message) {
    if (currentRoom) {
        auto msg_json = json_helper::message2Json(message, currentRoom->getName());
        QJsonDocument doc(msg_json);
        auto bytes = doc.toJson();
        for (const auto &peer: currentRoom->getPeers()) {
            std::cerr << "ip: " << peer.ip.toStdString() << " name " << peer.name.toStdString() << std::endl;
            auto socket = peer.getSocket();
            if (socket != nullptr && socket->isOpen()) {
                socket->write(bytes);
//                socket->flush();
                if (socket->waitForBytesWritten(3000)) {
                    std::cout << "Message sent: " << std::endl;
                } else {
                    std::cout << "Failed to send message: ";
                }
            }
        }
        return true;
    }
    return false;
}

void ClientManager::handleNewConnection() {
    QTcpSocket *clientSocket = tcp_server->nextPendingConnection();
    if (clientSocket != nullptr) {
        connect(clientSocket, &QTcpSocket::readyRead, this, [this, clientSocket]() {
            QByteArray data = clientSocket->readAll();
            qDebug() << "Received data:" << data;
            auto msg = json_helper::json2Message(QJsonDocument::fromJson(data).object());
            std::lock_guard guard{mutex};
            auto &target_room = roomList->find(msg.second).value();
            emit messageSent(msg.first.text);
            target_room.addMessage(std::move(msg.first));
        });

    }
}

void ClientManager::leaveRoom() {
    if (currentRoom) {
        currentRoom->removePeer(userName);
        currentRoom = nullptr;
    } else {
        qDebug() << "You are not in a room";
    }
}