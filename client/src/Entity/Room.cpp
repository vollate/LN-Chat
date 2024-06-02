#include "Room.hpp"

Room::Room(QString name, QString passWord){
    this->name = name;
    this->passWord = passWord;
}

void Room::addPeer(Peer&& peer){
    peers.append(peer);
}