#include "Peer.hpp"
#include "ClientManager.hpp"

Peer::Peer(QString name, QString ip) : name{name}, ip{ip} {}

std::unique_ptr<QTcpSocket> Peer::getSocket() const {
    auto ret = std::unique_ptr<QTcpSocket>();
    QHostAddress address{ip};
    ret->connectToHost(address, Client_Port);
    return ret;
}
