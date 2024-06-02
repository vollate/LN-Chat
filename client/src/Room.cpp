#include "Room.hpp"

#include <utility>

Room::Room(QString name, QString passWord){
    this->name = std::move(name);
    this->passWord = std::move(passWord);
}