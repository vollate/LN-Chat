#pragma once

#include <QString>
#include <qobject.h>

class Message {
public:
    Message() = default;

    Message(QString sender, QString time, QString text);

    // TODO: design message
    QString sender;
    QString time;
    QString text;
};