#include "messagethread.h"

MessageThread::MessageThread(const QString &contentInput,
                             const QList<QVariantMap> &context,
                             bool useStream,
                             QObject *parent)
    : QThread(parent),
      m_contentInput(contentInput),
      m_context(context),
      m_useStream(useStream)
{
}

void MessageThread::stop()
{
    terminate();
    wait();
}

void MessageThread::run()
{
    contentOutput = testText;
    if (m_useStream) {
        for (int i = 0; i < contentOutput.size(); i += 200) {
            emit newMessage(contentOutput.mid(i, 200));
            msleep(500);
        }
    } else {
        emit newMessage(contentOutput);
    }
    return;
}
