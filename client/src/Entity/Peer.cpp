#include "Peer.hpp"
#include "ClientManager.hpp"

Peer::Peer(QString name, QString ip) : name{name}, ip{ip} {}

std::unique_ptr<QTcpSocket> Peer::getSocket(quint16 port) const {
    auto ret = std::unique_ptr<QTcpSocket>();
    QHostAddress address{ip};
    ret->connectToHost(address, port);
    return ret;
}
