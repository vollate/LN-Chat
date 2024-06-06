#include "ClientController.hpp"
#include "ClientManager.hpp"
#include "ServerManager.hpp"

#include <random>

ClientController::ClientController(QObject* parent) : QObject(parent) {}

void ClientController::createRoom(ClientManager* clientManager, ServerManager* serverManager, QString roomName,
                                  QString password) {
    ClientManager::createRoom(roomName, password, serverManager);
    qDebug() << "Room " << roomName << " created";
}

void ClientController::connectToServer(ServerManager* serverManager, QString ip, QString port) {
    std::random_device rd;
    std::mt19937 gen(rd());
    int rand = gen();
    serverManager->setIp(ip);
    serverManager->setPort(port);
    serverManager->setName(QString::fromStdString("Client" + std::to_string(rand)));
    qDebug() << "Try to connect to ip: " << ip << " port: " << port;
    serverManager->startRpcClient();
    qDebug() << "Server ip: " << ip << " port: " << port;
}

void ClientController::joinRoom(ClientManager* clientManager, ServerManager* serverManager, QString roomName, QString password) {
    if(clientManager->joinRoom(roomName, password, serverManager)) {
        qDebug() << "Joined room " << roomName;
        emit roomJoined(roomName);
    } else {
        qDebug() << "Failed to join room";
    }
}

void ClientController::sendMessage(ClientManager* clientManager, QString messageText) {
    QDateTime now = QDateTime::currentDateTime();
    qint64 timestamp = now.toSecsSinceEpoch();
    QString timestampStr = QString::number(timestamp);
    Message message(clientManager->userName, timestampStr, messageText);
    if(clientManager->sendMessage(message)) {
        qDebug() << "Message sent " << messageText;
    } else {
        qDebug() << "Failed to send message";
    }
}

void ClientController::leaveRoom(ClientManager* clientManager) {
    clientManager->leaveRoom();
}

void ClientController::getCurrentRoomMsgs(ClientManager* clientManager) {
    qDebug() << "Current room messages: ";
    if(clientManager->currentRoom != nullptr) {
        auto msgs = clientManager->currentRoom->messages;
        for(const auto& msg : msgs) {
            qDebug() << msg.sender << ": " << msg.text;
        }
    }
}

    void ClientController::exportMessage(ClientManager* clientManager){
    
}
