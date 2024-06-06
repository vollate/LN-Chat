#include "Room.hpp"

#include <utility>

Room::Room(QString name, QString passWord) : name(std::move(name)), passWord(std::move(passWord)), peers{}, messages{} {}

void Room::addPeer(const Peer& peer) {
    if(havePeer(peer.ip)) {
        return;
    }
    peers.append(peer);
}

auto Room::getPeers() const -> QList<Peer> {
    return peers;
}

void Room::addMessage(const Message& message) {
    messages.append(message);
}

void Room::removePeer(const QString& name) {
    for(int i = 0; i < peers.size(); i++) {
        if(peers[i].name == name) {
            peers.removeAt(i);
            return;
        }
    }
}

auto Room::getName() const -> QString {
    return name;
}

auto Room::havePeer(const QString& ip) -> bool {
    return std::any_of(peers.begin(), peers.end(), [&ip](const Peer& peer) { return peer.ip == ip; });
}
