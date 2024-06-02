#pragma once
#include "ClientManager.hpp"

class ClientController{
public:
    ClientController();
    void createRoom(ClientManager* clientManager);
    void joinRoom(ClientManager* clientManager);
};