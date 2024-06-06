#pragma once

#include "Message.hpp"
#include "Peer.hpp"

#include <QJsonObject>
#include <QString>

namespace json_helper {
    QJsonObject message2Json(const Message& msg, const QString& room_name);

    QPair<Message, QString> json2Message(const QJsonObject& text);

    Peer json2Peer(const QJsonObject& peer);

}  // namespace json_helper

namespace ip_helper {
    std::string urlDecode(const std::string& encoded);

    std::string extractIPAddress(const std::string& fullAddress);
}  // namespace ip_helper
