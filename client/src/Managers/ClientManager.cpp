#include <memory>
#include <QJsonDocument>

#include "ClientManager.hpp"
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

void ClientManager::createRoom(QString name, QString password, ServerManager &serverManager) {
    serverManager.registerRoom(name.toStdString(), password.toStdString());
}

void ClientManager::joinRoom(QString name, QString password, QString username, ServerManager &serverManager) {
    if (auto peers_opt = serverManager.getPeers(name.toStdString(), password.toStdString())) {
        auto peers = peers_opt.value();
        auto room = std::make_shared<Room>(name, password);
        for (auto peer: peers) {
            room->addPeer(std::move(peer));
        }
        std::lock_guard guard{mutex};
        roomList->insert(name, *room);
        currentRoom = room;
        selfPeer = std::make_shared<Peer>(username, ip);
    } else {
        qDebug() << "Room not found or wrong password";
        return;
    }
}

ClientManager::~ClientManager() {
    tcp_server->close();
}

void ClientManager::sendMessage(const Message &message) {
    if (currentRoom && selfPeer) {
        auto msg_json = json_helper::message2Json(message, currentRoom->getName());
        QJsonDocument doc(msg_json);
        auto bytes = doc.toJson();
        for (const auto &peer: currentRoom->getPeers()) {
            auto socket = peer.getSocket();
            if (socket != nullptr && socket->isOpen()) {
                socket->write(bytes);
                socket->flush();
            }
        }
    }
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
            target_room.addMessage(msg.first);

            //TODO:@facooco emit signal to GUI
        });

    }
}
