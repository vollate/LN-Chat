#pragma once

#include <QList>
#include <QString>

#include "Message.hpp"
#include "Peer.hpp"
#include <QList>

class Room {
public:
    QString name;
    QString passWord;
    QList<Peer> peers;
    QList<Message> messages;

    Room(QString name, QString passWord);

    Room(const Room& room) = default;

    void addPeer(Peer&& peer);

    void removePeer(QString name);

    void addMessage(Message&& message);

    QList<Peer> getPeers();

    QString getName() const;
};
