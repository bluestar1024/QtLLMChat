#ifndef MESSAGETHREAD_H
#define MESSAGETHREAD_H

#pragma once
#include <QtCore/QThread>
#include <QtCore/QString>
#include <QtCore/QVariantMap>

class AppContext;

// 与 LLM 后端对话的工作线程（由 PyQt 版 messageThread 移植）：
// 构造时读取 config 文件中的连接参数与采样参数，run() 中以 OpenAI 兼容接口
// 发起 chat/completions 请求；流式模式逐段发射 newMessage，非流式模式在
// 响应完整返回后一次性发射
class MessageThread : public QThread
{
    Q_OBJECT
public:
    explicit MessageThread(AppContext *appContext, const QString &contentInput = "",
                           const QList<QVariantMap> &context = {}, bool useStream = true,
                           QObject *parent = nullptr);
    void stop();
    // 本线程是否被 stop() 主动停止过（持久记录，不受线程运行状态影响）：
    // QThread::isInterruptionRequested() 在线程结束（finished）后恒返回 false
    // （Qt 内部还要求 running && !finished && !isInFinish），无法用于线程结束后的
    // 收尾判断（如空文本自动重试的停止拦截），故自记标志。仅主线程读写
    // （stop() 与收尾逻辑均在主线程）
    bool isStoppedByUser() const { return stoppedByUser; }

signals:
    void newMessage(const QString &text);

protected:
    void run() override;

private:
    void loadConfig();

    AppContext *appContext;

    // LLM 配置（loadConfig 从 config 文件读取，文件缺失时回退 AppContext 默认值）
    QString baseUrl;
    QString apiKey;
    QString model;
    int maxTokensCurrentVal;
    double topPCurrentVal;
    double temperatureCurrentVal;

    QString contentInput;
    QList<QVariantMap> context;
    bool useStream;
    QString contentOutput;
    // stop() 被调用过（主动停止的持久记录，见 isStoppedByUser 说明）
    bool stoppedByUser = false;
};

#endif // MESSAGETHREAD_H
