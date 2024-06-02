#pragma once

#include <QString>
#include <QTcpSocket>

struct Peer {
    Peer(QString name, QString ip);

    [[nodiscard]] std::unique_ptr<QTcpSocket> getSocket()const;

    QString name;
    QString ip;
};