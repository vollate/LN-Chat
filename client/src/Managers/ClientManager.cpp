#include <memory>

#include "ClientManager.hpp"
#include "Room.hpp"
#include "ServerManager.hpp"

ClientManager::ClientManager()
        : QObject{parent()}, tcp_server{std::make_unique<QTcpServer>(this)},
          roomList{std::make_shared<QMap<QString, Room>>()},
          ip{"127.0.0.1"} {
    if (!tcp_server->listen(QHostAddress::Any, 12345)) {
        qCritical() << "Server could not start:" << tcp_server->errorString();
    } else {
        qDebug() << "Server started on port 12345";
        connect(tcp_server.get(), &QTcpServer::newConnection, this, &ClientManager::handleNewConnection);
    }
}

void ClientManager::createRoom(QString name, QString passWord, ServerManager &serverManager) {
    serverManager.registerRoom(name.toStdString(), passWord.toStdString());
}

void ClientManager::joinRoom(QString name, QString password, QString username, ServerManager &serverManager) {
    if (auto peers_opt = serverManager.getPeers(name.toStdString(), password.toStdString())) {
        auto peers = peers_opt.value();
        auto room = std::make_shared<Room>(name, password);
        for (auto peer: peers) {
            room->addPeer(std::move(peer));
        }
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

void ClientManager::sendMessage(const QByteArray &message) {
    if (currentRoom && selfPeer) {
        for (const auto &peer: currentRoom->getPeers()) {
            auto socket = peer.getSocket();
            if (socket != nullptr && socket->isOpen()) {
                socket->write(message);
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
            // Process data
        });

        // TODO
        //  Optionally add the new client to the current room or manage it as needed
        //  if(currentRoom) {
        //  currentRoom->addPeer(std::make_shared<Peer>(clientSocket));
        // }
    }
}
