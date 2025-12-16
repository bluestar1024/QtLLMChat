#ifndef MESSAGETHREAD_H
#define MESSAGETHREAD_H

#include <QThread>
#include <QString>
#include <QVariantMap>

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
    QString m_contentInput;
    QList<QVariantMap> m_context;
    bool m_useStream;
    QString contentOutput;
};

#endif // MESSAGETHREAD_H
