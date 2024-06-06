#pragma once

#include <QList>
#include <QString>

#include "Message.hpp"
#include "Peer.hpp"

class Room {
public:
    QString name;
    QString passWord;
    QList<Peer> peers;
    QList<Message> messages;

    Room(QString name, QString passWord);

    Room(const Room& room) = default;

    Room(Room&& room) = default;

    auto operator=(const Room& room) -> Room& = default;

    auto operator=(Room&& room) -> Room& = default;

    ~Room() = default;

    void addPeer(const Peer& peer);

    void removePeer(const QString& name);

    void addMessage(const Message& message);

    auto havePeer(const QString& name) -> bool;

    [[nodiscard]] auto getPeers() const -> QList<Peer>;

    [[nodiscard]] auto getName() const -> QString;
};
