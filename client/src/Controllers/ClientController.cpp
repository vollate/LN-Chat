#include "ClientController.hpp"
#include "ServerManager.hpp"

ClientController::ClientController(QObject* parent) : QObject(parent) {}

void ClientController::createRoom(ClientManager* clientManager, ServerManager* serverManager, QString roomName,
                                  QString password) {
    clientManager->createRoom(roomName, password, serverManager);
    qDebug() << "Room " << roomName << " created";
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
    if (clientManager->sendMessage(messageText)){
        qDebug () << "Message sent " << messageText;
        emit messageSent(messageText);
    }else{
        qDebug() << "Failed to send message";
    }
}

void ClientController::leaveRoom(ClientManager* clientManager) {
    clientManager->leaveRoom();
}
