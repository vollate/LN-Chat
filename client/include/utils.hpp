#pragma once

#include <QString>
#include <QJsonObject>
#include "Message.hpp"

namespace json_helper {
    QJsonObject message2Json(const Message &msg, const QString &room_name);

    QPair<Message, QString> json2Message(const QJsonObject &text);

}

namespace ip_helper {
    std::string urlDecode(const std::string &encoded);

    std::string extractIPAddress(const std::string &fullAddress);
}