#pragma once

#include <QString>
#include <QList>
#include <memory>

#include "Message.hpp"
#include "Peer.hpp"

class Room {
private:
    QString name;
    QString passWord;
    QList<std::shared_ptr<Peer>> peers;
    QList<Message> messages;

public:
    Room(QString name, QString passWord);

    void addPeer(std::shared_ptr<Peer> peer);

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

