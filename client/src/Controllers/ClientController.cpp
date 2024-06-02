#include "ClientController.hpp"
#include "ServerManager.hpp"

ClientController::ClientController(QObject *parent) : QObject(parent) {

}

void ClientController::createRoom(ClientManager& clientManager, ServerManager& serverManager, QString roomName, QString password) {
    clientManager.createRoom(roomName, password, serverManager);
    qDebug() << "Room " << roomName << " created";
}

void ClientController::joinRoom(ClientManager& clientManager, ServerManager& serverManager, QString roomName, QString password) {
    clientManager.joinRoom(roomName, password, serverManager);
}

void ClientController::sendMessage(ClientManager& clientManager) {
    clientManager.sendMessage("Hello");
}

void ClientController::leaveRoom(ClientManager& clientManager) {
    clientManager.leaveRoom();
}
