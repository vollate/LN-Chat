#pragma once

#include <QString>
#include <QList>

#include "Peer.hpp"
#include "Message.hpp"

class Peer;

class Room {
private:
    QString name;
    QString passWord;
    QList<Peer> peers;
    QList<Message> messages;
public:
    Room(QString name, QString passWord);

    void addPeer(Peer peer);

    void removePeer(Peer peer);

    void addMessage(Message message);

    QList<Peer> getPeers();

    QList<Message> getMessages();

    QString getName();

    QString getPassWord();

    void setName(QString name);

    void setPassWord(QString passWord);

    void setPeers(QList<Peer> peers);

    void setMessages(QList<Message> messages);
};

struct Peer {

public:
    Peer(QString name, QString ip);

    bool addToRoom(const Room &room);

    void sendMessage(QString message);

    QString name;
    QString ip;
};