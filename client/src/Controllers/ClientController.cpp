#include "ClientController.hpp"
#include "ClientManager.hpp"
#include "ServerManager.hpp"

ClientController::ClientController(QObject* parent) : QObject(parent) {}

void ClientController::createRoom(ClientManager* clientManager, ServerManager* serverManager, QString roomName,
                                  QString password) {
    clientManager->createRoom(roomName, password, serverManager);
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

QVariantList ClientController::getCurrentRoomMsgs(ClientManager* clientManager) {
    QList<Message> messages;
    QVariantList variantList;
    if (clientManager->currentRoom){
        messages = clientManager->currentRoom->messages;
        for (const auto& message : messages) {
            QVariantMap variantMap;
            variantMap["userName"] = message.sender;
            variantMap["timestamp"] = toReadableTime(message.time);
            variantMap["text"] = message.text;
            variantList.append(variantMap);
            qDebug() << "Sender: " << message.sender << " Text: " << message.text;
        }
    }
    return variantList;
}

void ClientController::setRoom(ClientManager* clientManager, QString roomName) {
    if (clientManager->roomList->contains(roomName)) {
        clientManager->currentRoom = clientManager->roomList->value(roomName);
    } else {
        qDebug() << "Room not found";
    }
}

void ClientController::exportMessage(ClientManager* clientManager){
    const auto& msgs = clientManager->currentRoom->messages;
    const auto& roomName = clientManager->currentRoom->getName();
    clientManager->exportMessage(msgs, "data/", roomName);
}

void ClientController::loadMessage(ClientManager* clientManager, const QString& roomName){
    clientManager->loadMessage("data/", roomName);
}

QString ClientController::toReadableTime(QString timestamp) {
    QDateTime time = QDateTime::fromSecsSinceEpoch(timestamp.toInt());
    return time.toString("yyyy-MM-dd hh:mm:ss");
}
