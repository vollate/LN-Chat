#include "utils.hpp"

QJsonObject json_helper::message2Json(const Message &msg, const QString &room_name) {
    QJsonObject message;
    message["timestamp"] = msg.time;
    message["roomName"] = room_name;
    message["sender"] = msg.sender;
    message["content"] = msg.text;
    return message;
}

QPair<Message, QString> json_helper::json2Message(const QJsonObject &text) {
    Message message;
    message.time = text["timestamp"].toString();
    message.text = text["text"].toString();
    message.sender = text["sender"].toString();
    return QPair<Message, QString>(message, text["roomName"].toString());

}