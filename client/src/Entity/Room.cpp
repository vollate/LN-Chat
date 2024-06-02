#include "Room.hpp"

Room::Room(QString name, QString passWord){
    this->name = name;
    this->passWord = passWord;
}

void Room::addPeer(std::shared_ptr<Peer> peer){
    peers.append(peer);
}

QString Room::getPassWord(){
    return passWord;
}