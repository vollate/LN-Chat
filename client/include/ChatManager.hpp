#pragma once

#include <QString>
#include <Peer.hpp>
#include <Room.hpp>

class ChatManager{
public:
    ChatManager();

private:
    Peer self;
    Room room;
};