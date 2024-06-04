#pragma once

#include "Message.hpp"
#include <QJsonObject>
#include <QString>
#include <sstream>


namespace json_helper {
    QJsonObject message2Json(const Message& msg, const QString& room_name);

    QPair<Message, QString> json2Message(const QJsonObject& text);

}  // namespace json_helper

namespace ip_helper {
    std::string urlDecode(const std::string& encoded);

    std::string extractIPAddress(const std::string& fullAddress);
}  // namespace ip_helper