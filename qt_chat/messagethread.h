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
};

#endif // MESSAGETHREAD_H
