#include "utils.hpp"
#include <istream>
#include <regex>
#include <string>
#include <sstream>

auto json_helper::message2Json(const Message& msg, const QString& room_name) -> QJsonObject {
    QJsonObject message;
    message["timestamp"] = msg.time;
    message["roomName"] = room_name;
    message["sender"] = msg.sender;
    message["content"] = msg.text;
    return message;
}

auto json_helper::json2Message(const QJsonObject& text) -> QPair<Message, QString> {
    Message message;
    message.time = text["timestamp"].toString();
    message.text = text["content"].toString();
    message.sender = text["sender"].toString();
    return QPair<Message, QString>(message, text["roomName"].toString());
}

auto json_helper::json2Peer(const QJsonObject& peer) -> Peer {
    auto name = peer["name"].toString();
    auto ip = peer["ip"].toString();
    return Peer(name, ip);
}

auto ip_helper::urlDecode(const std::string& encoded) -> std::string {
    std::string decoded;
    char tempChar = 0;
    int hex = 0;

    for(size_t i = 0; i < encoded.length(); ++i) {
        if(encoded[i] == '%' && i + 2 < encoded.length()) {
            std::stringstream ss;
            ss << std::iostream::hex << encoded.substr(i + 1, 2);
            ss >> hex;
            tempChar = static_cast<char>(hex);
            decoded += tempChar;
            i += 2;
        } else {
            decoded += encoded[i];
        }
    }
    return decoded;
}

auto ip_helper::extractIPAddress(const std::string& fullAddress) -> std::string {
    std::string decoded = urlDecode(fullAddress);
    std::smatch matches;
    std::regex ipv4_regex(R"(ipv4:([0-9]+(?:\.[0-9]+){3})(?::\d+)?)");
    std::regex ipv6_regex(R"(ipv6:\[([0-9a-fA-F:]+)\](?::\d+)?)");

    if(decoded.find("ipv4:") != std::string::npos) {
        if(std::regex_search(decoded, matches, ipv4_regex) && matches.size() > 1) {
            return matches[1].str();
        }
    } else if(decoded.find("ipv6:") != std::string::npos) {
        if(std::regex_search(decoded, matches, ipv6_regex) && matches.size() > 1) {
            return matches[1].str();
        }
    }

    return "";
}
