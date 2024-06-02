#pragma once

#include <QList>
#include <QString>

#include "Message.hpp"
#include "Peer.hpp"

class Room {

public:
    Room(QString name, QString passWord);

    void addPeer(Peer&& peer);

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

private:
    QString name;
    QString passWord;
    QList<Peer> peers;
    QList<Message> messages;
};
