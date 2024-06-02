#pragma once

#include "ClientManager.hpp"
#include "ServerManager.hpp"
class ClientController{
public:
    ClientController();
    void createRoom(ClientManager clientManager, ServerManager serverManager);
    void joinRoom(ClientManager clientManager, ServerManager serverManager);include/add_environment.h
    void leaveRoom(ClientManager clientManager);
    void sendMessage(ClientManager clientManager);
    void exportMessage(ClientManager clientManager);
};