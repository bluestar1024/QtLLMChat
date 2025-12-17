#ifndef MESSAGETHREAD_H
#define MESSAGETHREAD_H

#pragma once
#include <QtCore/QThread>
#include <QtCore/QString>
#include <QtCore/QVariantMap>

extern const QString testText;

class MessageThread : public QThread
{
    Q_OBJECT
public:
    explicit MessageThread(const QString &contentInput = "", const QList<QVariantMap> &context = {},
                           bool useStream = true, QObject *parent = nullptr);
    void stop();

signals:
    void newMessage(const QString &text);

protected:
    void run() override;

private:
    QString contentInput;
    QList<QVariantMap> context;
    bool useStream;
    QString contentOutput;
};

#endif // MESSAGETHREAD_H
