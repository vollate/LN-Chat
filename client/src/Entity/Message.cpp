#include "Message.hpp"

Message::Message(QObject *parent)
    : QObject(parent) {
}

Message::Message(const QString &sender, const QString &time, const QString &text, QObject *parent)
    : QObject(parent), m_sender(sender), m_time(time), m_text(text) {
}

Message::Message(const Message &other)
    : QObject(other.parent()), m_sender(other.m_sender), m_time(other.m_time), m_text(other.m_text) {
}

Message &Message::operator=(const Message &other) {
    if (this != &other) {
        m_sender = other.m_sender;
        m_time = other.m_time;
        m_text = other.m_text;
    }
    return *this;
}

QString Message::sender() const {
    return m_sender;
}

void Message::setSender(const QString &sender) {
    if (m_sender != sender) {
        m_sender = sender;
        emit senderChanged();
    }
}

QString Message::time() const {
    return m_time;
}

void Message::setTime(const QString &time) {
    if (m_time != time) {
        m_time = time;
        emit timeChanged();
    }
}

QString Message::text() const {
    return m_text;
}

void Message::setText(const QString &text) {
    if (m_text != text) {
        m_text = text;
        emit textChanged();
    }
}
