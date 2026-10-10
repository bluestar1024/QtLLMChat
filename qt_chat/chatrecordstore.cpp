#include "chatrecordstore.h"

#include <QtCore/QDateTime>
#include <QtCore/QDebug>
#include <QtCore/QDir>
#include <QtCore/QFileInfo>
#include <QtSql/QSqlError>
#include <QtSql/QSqlQuery>

ChatRecordStore::ChatRecordStore(const QString &dbPath)
{
    // 每个实例使用独立命名连接：避免默认连接被重复注册（窗口重建等场景），
    // 也避免与其它模块共用连接时的相互干扰
    static int connectionCounter = 0;
    connectionName = QString("chat_record_store_%1").arg(++connectionCounter);

    if (!QSqlDatabase::isDriverAvailable("QSQLITE")) {
        qDebug() << "聊天记录数据库不可用：缺少 QSQLITE 驱动";
        return;
    }

    // 数据库文件所在目录不存在时创建（聊天记录目录可能被整体清理）
    QDir().mkpath(QFileInfo(dbPath).absolutePath());

    db = QSqlDatabase::addDatabase("QSQLITE", connectionName);
    db.setDatabaseName(dbPath);
    if (!db.open()) {
        qDebug() << "打开聊天记录数据库失败：" << dbPath << db.lastError().text();
        return;
    }

    QSqlQuery query(db);
    bool ok = query.exec("CREATE TABLE IF NOT EXISTS chat_session ("
                         "    id         INTEGER PRIMARY KEY AUTOINCREMENT,"
                         "    created_at TEXT NOT NULL"
                         ")");
    ok = ok
            && query.exec("CREATE TABLE IF NOT EXISTS chat_message ("
                          "    id         INTEGER PRIMARY KEY AUTOINCREMENT,"
                          "    session_id INTEGER NOT NULL,"
                          "    seq        INTEGER NOT NULL,"
                          "    content    TEXT NOT NULL,"
                          "    think_time INTEGER NOT NULL DEFAULT 0,"
                          "    is_user    INTEGER NOT NULL DEFAULT 0"
                          ")");
    ok = ok
            && query.exec("CREATE INDEX IF NOT EXISTS idx_chat_message_session_seq "
                          "ON chat_message(session_id, seq)");
    if (!ok) {
        qDebug() << "创建聊天记录数据表失败：" << query.lastError().text();
        db.close();
    }
}

ChatRecordStore::~ChatRecordStore()
{
    const QString name = connectionName;
    if (db.isOpen())
        db.close();
    // 先释放本类持有的连接引用再移除，避免 Qt 警告连接仍在使用
    db = QSqlDatabase();
    QSqlDatabase::removeDatabase(name);
}

bool ChatRecordStore::isOpen() const
{
    return db.isOpen();
}

int ChatRecordStore::createSession()
{
    if (!isOpen())
        return -1;

    QSqlQuery query(db);
    query.prepare("INSERT INTO chat_session(created_at) VALUES(?)");
    query.addBindValue(QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss"));
    if (!query.exec()) {
        qDebug() << "新建聊天记录会话失败：" << query.lastError().text();
        return -1;
    }
    return query.lastInsertId().toInt();
}

bool ChatRecordStore::sessionExists(int sessionId) const
{
    if (!isOpen() || sessionId < 0)
        return false;

    QSqlQuery query(db);
    query.prepare("SELECT 1 FROM chat_session WHERE id = ?");
    query.addBindValue(sessionId);
    return query.exec() && query.next();
}

QList<ChatMessageRecord> ChatRecordStore::loadMessages(int sessionId) const
{
    QList<ChatMessageRecord> messages;
    if (!isOpen() || sessionId < 0)
        return messages;

    QSqlQuery query(db);
    query.prepare("SELECT content, think_time, is_user FROM chat_message "
                  "WHERE session_id = ? ORDER BY seq");
    query.addBindValue(sessionId);
    if (!query.exec()) {
        qDebug() << "加载聊天记录消息失败：" << query.lastError().text();
        return messages;
    }

    while (query.next()) {
        ChatMessageRecord record;
        record.content = query.value(0).toString();
        record.thinkTime = query.value(1).toInt();
        record.isUser = query.value(2).toInt() != 0;
        messages.append(record);
    }
    return messages;
}

bool ChatRecordStore::saveMessages(int sessionId, const QList<ChatMessageRecord> &messages)
{
    if (!isOpen() || sessionId < 0)
        return false;

    // 与库中现有内容逐条比较，定位首个差异位置：流式追加多为行内增长（消息数量与
    // 标记结构不变），仅按数量判断会漏写，必须比较完整内容；内容相同时不写库
    const QList<ChatMessageRecord> oldMessages = loadMessages(sessionId);
    const int oldSize = oldMessages.size();
    const int newSize = messages.size();
    const int commonSize = qMin(oldSize, newSize);
    int diffIndex = 0;
    while (diffIndex < commonSize && oldMessages.at(diffIndex) == messages.at(diffIndex))
        diffIndex++;
    if (diffIndex == oldSize && oldSize == newSize)
        return true;

    if (!db.transaction()) {
        qDebug() << "聊天记录事务开启失败：" << db.lastError().text();
        return false;
    }

    QSqlQuery delQuery(db);
    delQuery.prepare("DELETE FROM chat_message WHERE session_id = ? AND seq >= ?");
    delQuery.addBindValue(sessionId);
    delQuery.addBindValue(diffIndex);
    if (!delQuery.exec()) {
        qDebug() << "更新聊天记录失败：" << delQuery.lastError().text();
        db.rollback();
        return false;
    }

    QSqlQuery insertQuery(db);
    insertQuery.prepare("INSERT INTO chat_message(session_id, seq, content, think_time, is_user) "
                        "VALUES(?, ?, ?, ?, ?)");
    for (int i = diffIndex; i < newSize; i++) {
        const ChatMessageRecord &record = messages.at(i);
        insertQuery.addBindValue(sessionId);
        insertQuery.addBindValue(i);
        insertQuery.addBindValue(record.content);
        insertQuery.addBindValue(record.thinkTime);
        insertQuery.addBindValue(record.isUser ? 1 : 0);
        if (!insertQuery.exec()) {
            qDebug() << "更新聊天记录失败：" << insertQuery.lastError().text();
            db.rollback();
            return false;
        }
    }

    if (!db.commit()) {
        qDebug() << "提交聊天记录失败：" << db.lastError().text();
        db.rollback();
        return false;
    }
    return true;
}

QList<ChatSessionRecord> ChatRecordStore::listSessions(const QString &searchText) const
{
    QList<ChatSessionRecord> sessions;
    if (!isOpen())
        return sessions;

    // instr 按字面匹配（无 LIKE 通配符转义问题），等价 QString::contains 的区分大小写语义
    QString sql = "SELECT id FROM chat_session";
    if (!searchText.isEmpty())
        sql += " WHERE id IN (SELECT session_id FROM chat_message WHERE instr(content, ?) > 0)";
    sql += " ORDER BY created_at, id";

    QSqlQuery query(db);
    query.prepare(sql);
    if (!searchText.isEmpty())
        query.addBindValue(searchText);
    if (!query.exec()) {
        qDebug() << "加载聊天记录列表失败：" << query.lastError().text();
        return sessions;
    }

    while (query.next()) {
        const int sessionId = query.value(0).toInt();
        ChatSessionRecord record;
        record.id = sessionId;
        record.title = sessionTitle(sessionId);
        if (record.title.isEmpty())
            continue;
        sessions.append(record);
    }
    return sessions;
}

void ChatRecordStore::deleteAllSessions()
{
    if (!isOpen())
        return;

    QSqlQuery query(db);
    if (!query.exec("DELETE FROM chat_message") || !query.exec("DELETE FROM chat_session"))
        qDebug() << "删除全部聊天记录失败：" << query.lastError().text();
    else
        qDebug() << "删除全部聊天记录成功";
}

QString ChatRecordStore::sessionTitle(int sessionId) const
{
    // 一条查询取前两条消息（seq 升序）：本项目逻辑下首条必为用户消息、第二条为 AI
    // 回复，两者首行拼接为标题（对应原实现"文件第一行 + True 标记后一行"）；
    // 任一段缺失返回空串
    QSqlQuery query(db);
    query.prepare("SELECT content FROM chat_message WHERE session_id = ? ORDER BY seq LIMIT 2");
    query.addBindValue(sessionId);
    if (!query.exec() || !query.next())
        return QString();
    const QString firstLine = query.value(0).toString().section('\n', 0, 0);

    // 第二条消息（AI 回复）：首行为 <think> 时取下一行
    if (!query.next())
        return firstLine + '\n';
    const QStringList lines = query.value(0).toString().split('\n');
    QString aiLine = lines.value(0);
    if (aiLine == "<think>")
        aiLine = lines.value(1);
    if (aiLine.isEmpty())
        return QString();

    return firstLine + '\n' + aiLine;
}
