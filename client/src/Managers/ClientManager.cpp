#include <QDir>
#include <QFile>
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

auto ClientManager::sendMessage(const Message& message) -> bool {
    if(currentRoom) {
        auto msg_json = json_helper::message2Json(message, currentRoom->getName());
        QJsonDocument doc(msg_json);
        auto bytes = doc.toJson();
        for(const auto& peer : currentRoom->getPeers()) {
            qDebug() << "ip: " << peer.ip << " name " << peer.name;
            auto socket = peer.getSocket();
            if(socket != nullptr && socket->isOpen()) {
                socket->write(bytes);
                //                socket->flush();
                if(socket->waitForBytesWritten(3000)) {
                    std::cout << "Message sent: " << std::endl;
                    Message rcvMsg{ message.sender, message.time, message.text };
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

void ClientManager::exportMessage(const QList<Message>& texts, const QString& path, const QString& chatroomName) {
    QJsonArray messagesArray;
    for(int i = 0; i < texts.size(); ++i) {
        QJsonObject m;
        m["message_id"] = i;
        m["timestamp"] = texts[i].time;
        m["sender"] = texts[i].sender;
        m["content"] = texts[i].text;
        messagesArray.append(m);
    }
    QString fileName = "chatroom-" + chatroomName + ".json";
    QString filePath = QDir(path).filePath(fileName);

    QJsonObject chatroomObject;
    chatroomObject["name"] = chatroomName;
    chatroomObject["messages"] = messagesArray;

    QJsonObject mainObject;
    mainObject["chatroom"] = chatroomObject;

    QJsonDocument jsonDocument(mainObject);

    QDir dir(path);
    if(!dir.exists()) {
        if(!dir.mkpath(".")) {
            qDebug() << "Couldn't create directory.";
            return;
        }
    }

    QFile file(filePath);
    if(!file.open(QIODevice::WriteOnly)) {
        qDebug() << "Couldn't open file for writing.";
        return;
    }

    file.write(jsonDocument.toJson());
    file.close();
}

void ClientManager::loadMessage(const QString& path, const QString& name) {
    QString fileName = "chatroom-" + name + ".json";
    QString filePath = QDir(path).filePath(fileName);
    QFile file(filePath);
    if(!file.open(QIODevice::ReadOnly)) {
        qDebug() << "Have no such doc";
        emit fileError(name);
        return;
    }
    QByteArray fileData = file.readAll();
    file.close();

    QJsonDocument jsonDoc = QJsonDocument::fromJson(fileData);
    if(!jsonDoc.isObject()) {
        qDebug() << "Failed to parse JSON file";
        emit fileError(name);
        return;
    }
    QJsonObject jsonObj = jsonDoc.object();
    QVariantList mList;
    if(jsonObj.contains("chatroom") && jsonObj["chatroom"].isObject()) {
        QJsonObject chatroomObj = jsonObj["chatroom"].toObject();
        if(chatroomObj.contains("messages") && chatroomObj["messages"].isArray()) {
            QJsonArray messagesArray = chatroomObj["messages"].toArray();
            for(const QJsonValue& messageVal : messagesArray) {
                if(messageVal.isObject()) {
                    QJsonObject messageObj = messageVal.toObject();
                    QVariantMap messageMap;
                    messageMap["content"] = messageObj["content"].toString();
                    messageMap["message_id"] = messageObj["message_id"].toInt();
                    messageMap["timestamp"] = messageObj["timestamp"].toString();
                    messageMap["sender"] = messageObj["sender"].toString();
                    mList.append(messageObj);
                } else {
                    qDebug() << "Failed to parse JSON file";
                    emit fileError(name);
                    return;
                }
            }
        } else {
            qDebug() << "Failed to parse JSON file";
            emit fileError(name);
            return;
        }
        emit fileLoaded(mList, name);
        return;
    } else {
        qDebug() << "Failed to parse JSON file";
        emit fileError(name);
        return;
    }
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
