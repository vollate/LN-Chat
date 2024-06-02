#pragma once

#include <QObject>
#include <QString>

class Message : public QObject {
    Q_OBJECT
    Q_PROPERTY(QString sender READ sender WRITE setSender NOTIFY senderChanged)
    Q_PROPERTY(QString time READ time WRITE setTime NOTIFY timeChanged)
    Q_PROPERTY(QString text READ text WRITE setText NOTIFY textChanged)

public:
    explicit Message(QObject *parent = nullptr);
    Message(const QString &sender, const QString &time, const QString &text, QObject *parent = nullptr);

    Message(const Message&);
    Message& operator=(const Message&);
    
    QString sender() const;
    void setSender(const QString &sender);

    QString time() const;
    void setTime(const QString &time);

    QString text() const;
    void setText(const QString &text);

signals:
    void senderChanged();
    void timeChanged();
    void textChanged();

private:
    QString m_sender;
    QString m_time;
    QString m_text;
};

