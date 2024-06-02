#pragma once

#include <QString>
#include <QTcpSocket>
#include "Constant.hpp"

struct Peer {
    Peer(QString name, QString ip);

    [[nodiscard]] std::unique_ptr<QTcpSocket> getSocket(quint16 port = Client_Server_Port) const;

    QString name;
    QString ip;
};