#pragma once
#include "RoomManager.hpp"

class RoomController{
public:
    RoomController();
    void createRoom(RoomManager* roomManager);
    void joinRoom(RoomManager* roomManager);
};