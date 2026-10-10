#ifndef CHATRECORDSTORE_H
#define CHATRECORDSTORE_H

#pragma once
#include <QtCore/QList>
#include <QtCore/QString>
#include <QtSql/QSqlDatabase>

// 单条聊天消息：对应原 txt 记录中"消息文本 + 思考时长 + 用户标记"结构
struct ChatMessageRecord
{
    QString content; // 消息文本（不含末尾换行）
    int thinkTime = 0; // 消息部件思考时长（秒）
    bool isUser = false;

    bool operator==(const ChatMessageRecord &other) const
    {
        return content == other.content && thinkTime == other.thinkTime && isUser == other.isUser;
    }
};

// 会话摘要：聊天历史列表展示用
struct ChatSessionRecord
{
    int id = -1;
    QString title;
};

// 聊天记录数据库存取（QSQLITE）：chat_session 存会话、chat_message 存消息，
// 替代原"一个会话一个 txt 文件"的存储方式；对外接口语义与原文件读写保持一致
class ChatRecordStore
{
public:
    explicit ChatRecordStore(const QString &dbPath);
    ~ChatRecordStore();

    bool isOpen() const;

    // 新建会话（fileName 沿用 chat_时间戳.txt 命名，仅作会话标识），返回会话 id，失败返回 -1
    int createSession(const QString &fileName);

    bool sessionExists(int sessionId) const;

    // 加载会话的全部消息（按 seq 升序）
    QList<ChatMessageRecord> loadMessages(int sessionId) const;

    // 保存会话的全部消息：与库中现有内容逐条比较，仅从首个差异位置起删除重建（事务）。
    // 内容完全相同时不写库（同原"完整内容比较"策略：流式追加多为行内增长、消息数量不变）
    bool saveMessages(int sessionId, const QList<ChatMessageRecord> &messages);

    // 会话列表，按创建时间升序（调用方头插展示最新在上）；searchText 非空时按消息内容
    // 过滤（等价原 txt 整文件 contains）；无法生成标题的会话跳过（等价原解析失败不显示）
    QList<ChatSessionRecord> listSessions(const QString &searchText = QString()) const;

    // 删除全部会话及其消息
    void deleteAllSessions();

private:
    // 生成会话列表标题：首条消息首行 + 首个用户消息之后一条消息首行（首行为 <think>
    // 时取下一行），对应原实现"文件第一行 + True 标记后一行"；任一段缺失返回空串
    QString sessionTitle(int sessionId) const;

    QSqlDatabase db;
    QString connectionName;
};

#endif // CHATRECORDSTORE_H
