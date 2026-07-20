#include "messagethread.h"
#include "globalvariables.h"

MessageThread::MessageThread(const QString &contentInput, const QList<QVariantMap> &context,
                             bool useStream, QObject *parent)
    : QThread(parent), contentInput(contentInput), context(context), useStream(useStream)
{
}

void MessageThread::stop()
{
    terminate();
    wait();
}

void MessageThread::run()
{
    contentOutput = testText_simple;
    if (useStream) {
        for (int i = 0; i < contentOutput.size(); i += 30) {
            emit newMessage(contentOutput.mid(i, 30));
            msleep(500);
        }
    } else {
        emit newMessage(contentOutput);
    }
    return;
}
