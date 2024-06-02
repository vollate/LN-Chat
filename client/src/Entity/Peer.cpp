#include "Peer.hpp"
#include "ClientManager.hpp"

Peer::Peer(QString name, QString ip) : name{std::move(name)}, ip{std::move(ip)} {}

std::unique_ptr<QTcpSocket> Peer::getSocket(quint16 port) const {
    auto socket = std::make_unique<QTcpSocket>();
    QHostAddress address{ip};
    socket->connectToHost(address, port);
    socket->waitForConnected(3000);
    return socket;
}
