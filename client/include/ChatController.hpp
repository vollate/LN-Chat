#pragma once

class ChatController{
public:
    ChatController();

    void loadChat();

    void sendMessage();

    // for remote function call
    void receiveMessage();

    void exportChat();

    void leaveRoom();

    void startVote();

    void endVote();
};