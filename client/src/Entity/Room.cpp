#include "Room.hpp"

Room::Room(QString name, QString passWord) : name{name}, passWord{passWord} {}

void Room::addPeer(Peer &&peer) {
    peers.append(peer);
}

QList<Peer> Room::getPeers() {
    return peers;
}

QString Room::getName() {
    return name;
}

void Room::addMessage(Message message) {
    messages.append(std::move(message));
}
