#include "messagethread.h"
#include "appcontext.h"

#include <QtCore/QDebug>
#include <QtCore/QEventLoop>
#include <QtCore/QFile>
#include <QtCore/QJsonArray>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>
#include <QtCore/QStringConverter>
#include <QtCore/QTextStream>
#include <QtCore/QTimer>
#include <QtCore/QUrl>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>
#include <QtNetwork/QNetworkRequest>

MessageThread::MessageThread(AppContext *appContext, const QString &contentInput,
                             const QList<QVariantMap> &context, bool useStream, QObject *parent)
    : QThread(parent),
      appContext(appContext),
      contentInput(contentInput),
      context(context),
      useStream(useStream)
{
    // 与 PyQt 版一致：构造时读取配置（每次发送都会新建线程，配置改动即时生效）
    loadConfig();
}

void MessageThread::loadConfig()
{
    // 先取 AppContext 默认值：config 文件缺失或内容不足时回退，避免参数为空
    baseUrl = appContext->initBaseUrl();
    apiKey = appContext->initApiKey();
    model = appContext->initModel();
    maxTokensCurrentVal = appContext->initMaxTokensCurrentVal();
    topPCurrentVal = appContext->initTopPCurrentVal();
    temperatureCurrentVal = appContext->initTemperatureCurrentVal();

    QFile configFile(appContext->configFilePath());
    if (!configFile.exists()) {
        qDebug() << "错误：文件" << appContext->configFilePath() << "不存在";
        return;
    }
    if (!configFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "错误：无法打开文件" << appContext->configFilePath();
        return;
    }

    QTextStream in(&configFile);
    in.setEncoding(QStringConverter::Utf8);
    QStringList content;
    while (!in.atEnd()) {
        content.append(in.readLine());
    }
    configFile.close();

    if (content.size() >= 6) {
        baseUrl = content[0].trimmed();
        apiKey = content[1].trimmed();
        model = content[2].trimmed();
        maxTokensCurrentVal = content[3].trimmed().toInt();
        topPCurrentVal = content[4].trimmed().toDouble();
        temperatureCurrentVal = content[5].trimmed().toDouble();
    }
}

void MessageThread::stop()
{
    // 与 PyQt 版 terminate+wait 保持相同的同步语义：调用返回时线程已结束。
    // 但优先温和中止：requestInterruption 后由 run() 内的看门狗中止网络请求
    // 并让事件循环自然退出，避免 terminate 硬杀正在跑事件循环的线程可能残留
    // 网络栈/CRT 内部锁（本项目此前多次崩溃分析均指向硬杀线程的风险）；
    // 3 秒未退出再 terminate 兜底
    requestInterruption();
    if (!wait(3000)) {
        terminate();
        wait();
    }
}

void MessageThread::run()
{
    QNetworkAccessManager manager;

    // base_url 末尾斜杠可有可无（config 中记录为 http://127.0.0.1:11434/v1）
    QString baseUrlNormalized = baseUrl;
    if (!baseUrlNormalized.endsWith('/')) {
        baseUrlNormalized += '/';
    }

    QNetworkRequest request(QUrl(baseUrlNormalized + "chat/completions"));
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setRawHeader("Authorization", "Bearer " + apiKey.toUtf8());

    // 消息列表：历史上下文在前，本轮输入在后（与 PyQt 版 context + text 一致）
    QJsonArray messages;
    for (const QVariantMap &item : context) {
        QJsonObject message;
        message["role"] = item.value("role").toString();
        message["content"] = item.value("content").toString();
        messages.append(message);
    }
    QJsonObject input;
    input["role"] = "user";
    input["content"] = contentInput;
    messages.append(input);

    QJsonObject body;
    body["model"] = model;
    body["messages"] = messages;
    body["stream"] = useStream;
    body["max_tokens"] = maxTokensCurrentVal;
    body["temperature"] = temperatureCurrentVal;
    body["presence_penalty"] = 1.1;
    body["top_p"] = topPCurrentVal;

    QNetworkReply *reply =
            manager.post(request, QJsonDocument(body).toJson(QJsonDocument::Compact));

    QByteArray buffer;        // 流式 SSE 行缓冲（不完整的残行留存至下次 readyRead）
    QByteArray responseData;  // 非流式响应体累积（自有缓冲，reply 设备关闭后仍可用）
    // 解析一行 SSE 事件（data: {json} / data: [DONE]），取 choices[0].delta.content；
    // 与 PyQt 版一致只转发 content 字段
    auto processLine = [&](const QByteArray &line) {
        if (!line.startsWith("data:")) {
            return;
        }
        const QByteArray data = line.mid(5).trimmed();
        if (data.isEmpty() || data == "[DONE]") {
            return;
        }
        const QJsonArray choices =
                QJsonDocument::fromJson(data).object().value("choices").toArray();
        if (choices.isEmpty()) {
            return;
        }
        const QString content =
                choices.at(0).toObject().value("delta").toObject().value("content").toString();
        if (!content.isEmpty()) {
            contentOutput += content;
            emit newMessage(content);
        }
    };

    QEventLoop loop;

    if (useStream) {
        // SSE 增量到达：按行切分，不完整的残行留在 buffer 等待下次 readyRead 补齐
        QObject::connect(reply, &QNetworkReply::readyRead, reply, [&]() {
            buffer.append(reply->readAll());
            while (true) {
                const int pos = buffer.indexOf('\n');
                if (pos < 0) {
                    break;
                }
                processLine(buffer.left(pos).trimmed());
                buffer.remove(0, pos + 1);
            }
        });
    } else {
        // 非流式同样主动把数据读入自有缓冲：reply 被中止（stop()）关闭设备后
        // readAll 会失败，只有自有缓冲能保住已到达的响应体；并且一旦能解析出
        // 完整响应结构即提前退出事件循环——服务器不发送结束标记（无 Content-Length、
        // 无 chunked 终止且不关闭连接）时 finished 可能永不到达，不能把"退出事件循环
        // 并发送 newMessage"这件事绑定在 HTTP 层结束标记或外部 stop() 上
        QObject::connect(reply, &QNetworkReply::readyRead, reply, [&]() {
            responseData.append(reply->readAll());
            const QJsonArray choices =
                    QJsonDocument::fromJson(responseData).object().value("choices").toArray();
            if (!choices.isEmpty()
                && choices.at(0).toObject().value("message").toObject().contains("content")) {
                loop.quit();
            }
        });
    }

    // 看门狗：stop() 的 requestInterruption 在网络静默（迟迟无 readyRead）时无法
    // 唤醒事件循环，由定时器周期检查并中止请求，使 run() 尽快自然退出
    QTimer watchdog;
    QObject::connect(&watchdog, &QTimer::timeout, reply, [&]() {
        if (isInterruptionRequested()) {
            reply->abort();
        }
    });
    watchdog.start(100);

    QObject::connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
    // finished 也可能在 post() 返回前就已同步发出（Qt 内部错误路径）：此时 exec 永远
    // 等不到退出条件，只能靠外部 stop() 中止——先检查再进入事件循环
    if (!reply->isFinished()) {
        loop.exec();
    }
    watchdog.stop();

    if (useStream) {
        // 收尾：处理最后一行未以换行结束的残留数据（正常 [DONE] 之后 buffer 为空）
        buffer.append('\n');
        while (true) {
            const int pos = buffer.indexOf('\n');
            if (pos < 0) {
                break;
            }
            processLine(buffer.left(pos).trimmed());
            buffer.remove(0, pos + 1);
        }
    } else if (reply->error() == QNetworkReply::NoError) {
        // 非流式：一次性取 choices[0].message.content 发射（与 PyQt 版一致）。
        // 优先用自有缓冲（提前退出事件循环时 reply 可能尚未完成，设备关闭后 readAll 不可用）
        QByteArray data = responseData;
        if (data.isEmpty() && reply->isOpen()) {
            data = reply->readAll();
        }
        const QJsonArray choices = QJsonDocument::fromJson(data).object().value("choices").toArray();
        if (!choices.isEmpty()) {
            contentOutput = choices.at(0)
                                    .toObject()
                                    .value("message")
                                    .toObject()
                                    .value("content")
                                    .toString();
            emit newMessage(contentOutput);
        }
    }

    // 主动中止（stop()）不视为错误；其余异常打印日志（对应 PyQt 版的 print 错误）
    if (reply->error() != QNetworkReply::NoError
        && reply->error() != QNetworkReply::OperationCanceledError) {
        qDebug() << "错误：HTTP"
                 << reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt()
                 << reply->errorString();
    }

    // 非流式因"响应已完整"提前退出事件循环时 reply 可能仍未 finished：主动中止以释放连接
    // （此时输出已完成，abort 触发的 finished(OperationCanceled) 已无人等待）
    if (!reply->isFinished()) {
        reply->abort();
    }

    delete reply;
}
