#include "thinkwidget.h"
#include "appcontext.h"

#include <QDebug>
#include <QPointer>

#include <cmath>

ThinkWidget::ThinkWidget(AppContext *appContext, const QString &text,
                         std::function<void()> sizeFinishFun, int maxWidth, QWidget *parent)
    : QWidget(parent),
      appContext(appContext),
      text(text.trimmed()),
      sizeFinishFun(sizeFinishFun),
      maxWidth(maxWidth - 10),
      // isLabel(true),
      isSetTextEnd(false),
      // isEmitSizeFinish(false),
      isSizeFinish(false),
      isPageLoaded(false),
      invalidSizeCount(0)
{
    connect(this, &ThinkWidget::setSizeFinished, this->sizeFinishFun);

    bool fontLoaded = false;
    // 字体族由 AppContext 统一注册缓存：ThinkWidget 在会话切换/窗口重建中
    // 大量创建，逐个自行 addApplicationFont 会使字体数据库重复累积
    const QString &fontFamily = this->appContext->fontFamily();
    if (!fontFamily.isEmpty()) {
        font = QFont(fontFamily);
        font.setPixelSize(this->appContext->windowFontPixelSize());
        fontLoaded = true;
    }
    // label = new CustomLabel();
    // label->setTextInteractionFlags(Qt::TextSelectableByMouse);
    // label->setWordWrap(true);
    // label->setMaximumWidth(this->maxWidth);
    // if (fontLoaded) {
    //     fontMetrics = new QFontMetricsF(font);
    //     label->setFont(font);
    // } else {
    //     fontMetrics = new QFontMetricsF(label->font());
    // }

    updateSizeTimer = new QTimer(this);
    updateSizeTimer->setSingleShot(true);
    connect(updateSizeTimer, &QTimer::timeout, this, &ThinkWidget::onUpdateSize);

    // 构造时即建立父子链（不依赖后续 addWidget）：控件随父销毁时 view 必然在内；
    // 若视图游离（无父）期间父链被摘除/销毁，view 会以顶层窗口存活并继续产生事件
    webEngineView = new WebEngineView(this->appContext, this);
    webEngineView->setMaximumWidth(this->maxWidth);
    connect(webEngineView->page(), &QWebEnginePage::loadFinished, this,
            &ThinkWidget::onPageLoadFinished);
    connect(webEngineView->page(), &QWebEnginePage::contentsSizeChanged, this,
            &ThinkWidget::onContentsSizeChanged);

    // isLabel = true;
    mainHLayout = new QHBoxLayout(this);
    // mainHLayout->addWidget(label);
    mainHLayout->setContentsMargins(5, 0, 5, 0);
    // setText(this->text);

    htmlText.clear();
    fullHtmlText.clear();

    if (fontLoaded) {
        fontMetrics = new QFontMetricsF(font);
    } else {
        fontMetrics = new QFontMetricsF(webEngineView->font());
    }

    // webEngineView->hide();
    int initWidth = int(fontMetrics->horizontalAdvance(text));
    if (initWidth > maxWidth)
        webEngineView->setFixedWidth(maxWidth);
    else {
        if (text.isEmpty())
            webEngineView->setFixedSize(20, 66);
        else
            webEngineView->setFixedWidth(initWidth);
    }
    // ---- MathJax 头 ----
    mathJaxCdn = QString(R"(
<!DOCTYPE html>
<html lang="en">
<head>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width, initial-scale=1.0">
<script>
MathJax = {
  options: { enableMenu: false },
  tex: { inlineMath:[["$","$"],["\ $","\$"]], displayMath:[["$$","$$"],["\
 $$","\$$ "]] },
  svg:  { fontCache: 'global' }
};
</script>
<script src="%1"></script>
<style>
table{width:50%;border-collapse:collapse;margin:10px 0;}
th,td{border:1px solid #000;padding:8px;}
th{background-color:#b0b0b0;}
.left-align{text-align:left;}
.center-align{text-align:center;}
.right-align{text-align:right;}
body,html{margin:0;padding:0;width:100%;height:100%;box-sizing:border-box;font-size:%2px;overflow:hidden;}
.content{width:auto;height:auto;display:flex;flex-direction:column;justify-content:center;}
</style>
</head>
<body>
<div class="content">
)")
                         .arg(this->appContext->mathjaxScriptPath())
                         .arg(this->appContext->windowFontPixelSize());
    // ---- markdown → html ----
    if (!text.isEmpty()) {
        htmlText = buildHtmlText(text);
        fullHtmlText = mathJaxCdn + htmlText + "</div></body></html>";
        // QUrl base = QUrl::fromLocalFile(QFileInfo(".").absolutePath() + "/");
        QUrl base =
                QUrl::fromLocalFile(QFileInfo(QFileInfo(".").absolutePath()).absolutePath() + "/");
        qDebug() << "base:" << base << this;

        // mainHLayout->removeWidget(label);
        // label->deleteLater();
        mainHLayout->addWidget(webEngineView);
        // webEngineView->hide();
        webEngineView->setHtml(fullHtmlText, base);
        // isLabel = false;

        // QSize webEngineSize = webEngineView->page()->contentsSize().toSize();
        // webEngineView->setFixedSize(webEngineSize);
        // setFixedSize(webEngineSize + QSize(10, 0));
        // qDebug() << "webEngineSize:" << webEngineSize << this;
    } else {
        // mainHLayout->removeWidget(label);
        // label->deleteLater();
        mainHLayout->addWidget(webEngineView);
        setFixedSize(webEngineView->size() + QSize(10, 0));
        emit setSizeFinished();
        // isLabel = false;
    }
    isSetTextEnd = true;
    qDebug() << "ThinkWidget init end" << this;
}

ThinkWidget::~ThinkWidget() { }

// 旧控件被摘除/即将销毁前调用（新建聊天、切换记录、窗口重建）：
// 停止尺寸探测定时器、中止页面加载并断开 page 信号，使其不再产生新的
// 异步回调。销毁前的窗口期内（deleteLater 尚未执行），旧的加载/尺寸回调
// 若进入后续事件循环会在新会话渲染的嵌套等待中被处理，与新控件活动叠加
void ThinkWidget::stopPendingWork()
{
    updateSizeTimer->stop();
    webEngineView->stop();
    if (webEngineView->page())
        webEngineView->page()->disconnect(this);
}

void ThinkWidget::setText(const QString &text)
{
    // this->text = text.trimmed();
    // int labelWidth = 0, labelHeight = 0;
    // if (!this->text.isEmpty()) {
    //     measureText(this->text, labelWidth, labelHeight);
    //     label->setText(this->text);
    //     label->setFixedSize(labelWidth, labelHeight);
    //     setFixedSize(labelWidth + 10, labelHeight);
    // } else {
    //     int h = int(fontMetrics->height());
    //     label->setFixedSize(h, h);
    //     setFixedSize(label->size() + QSize(10, 0));
    // }

    this->text = text.trimmed();
    htmlText.clear();
    fullHtmlText.clear();

    int initWidth = int(fontMetrics->horizontalAdvance(text));
    if (initWidth > maxWidth)
        webEngineView->setFixedWidth(maxWidth);
    else {
        if (text.isEmpty())
            webEngineView->setFixedSize(20, 66);
        else
            webEngineView->setFixedWidth(initWidth);
    }
    // ---- MathJax 头 ----
    mathJaxCdn = QString(R"(
<!DOCTYPE html>
<html lang="en">
<head>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width, initial-scale=1.0">
<script>
MathJax = {
  options: { enableMenu: false },
  tex: { inlineMath:[["$","$"],["\ $","\$"]], displayMath:[["$$","$$"],["\
 $$","\$$ "]] },
  svg:  { fontCache: 'global' }
};
</script>
<script src="%1"></script>
<style>
table{width:50%;border-collapse:collapse;margin:10px 0;}
th,td{border:1px solid #000;padding:8px;}
th{background-color:#b0b0b0;}
.left-align{text-align:left;}
.center-align{text-align:center;}
.right-align{text-align:right;}
body,html{margin:0;padding:0;width:100%;height:100%;box-sizing:border-box;font-size:%2px;overflow:hidden;}
.content{width:auto;height:auto;display:flex;flex-direction:column;justify-content:center;}
</style>
</head>
<body>
<div class="content">
)")
                         .arg(appContext->mathjaxScriptPath())
                         .arg(appContext->windowFontPixelSize());
    // ---- markdown → html ----
    if (!text.isEmpty()) {
        htmlText = buildHtmlText(text);
        fullHtmlText = mathJaxCdn + htmlText + "</div></body></html>";
        // QUrl base = QUrl::fromLocalFile(QFileInfo(".").absolutePath() + "/");
        QUrl base =
                QUrl::fromLocalFile(QFileInfo(QFileInfo(".").absolutePath()).absolutePath() + "/");
        qDebug() << "base:" << base << this;

        // mainHLayout->removeWidget(label);
        // label->deleteLater();
        // mainHLayout->addWidget(webEngineView);
        webEngineView->setHtml(fullHtmlText, base);

        // QSize webEngineSize = webEngineView->page()->contentsSize().toSize();
        // webEngineView->setFixedSize(webEngineSize);
        // setFixedSize(webEngineSize + QSize(10, 0));
        // qDebug() << "webEngineSize:" << webEngineSize << this;
    } else {
        // mainHLayout->removeWidget(label);
        // label->deleteLater();
        mainHLayout->addWidget(webEngineView);
        setFixedSize(webEngineView->size() + QSize(10, 0));
        emit setSizeFinished();
        // isLabel = false;
    }
    isSetTextEnd = true;
}

void ThinkWidget::measureText(const QString &text, int &labelWidth, int &labelHeight) const
{
    int textHeight = int(fontMetrics->height());
    QStringList lines = text.split('\n');
    int maxLineWidth = 0;
    for (const QString &line : lines)
        maxLineWidth = qMax(maxLineWidth, int(fontMetrics->horizontalAdvance(line)));

    if (maxLineWidth + 4 < maxWidth) {
        labelWidth = maxLineWidth + 4;
        labelHeight = lines.size() * (textHeight + 3) - 3;
    } else {
        int totalWidth = 0;
        for (int i = 0; i < lines.size(); ++i) {
            qreal w = fontMetrics->horizontalAdvance(lines[i] + (i < lines.size() - 1 ? " " : ""));
            totalWidth += std::ceil(w / (maxWidth - 24)) * (maxWidth - 24);
        }
        labelWidth = maxWidth;
        labelHeight = std::ceil(totalWidth / qreal(maxWidth - 24)) * (textHeight + 3) - 3;
    }
}

QString ThinkWidget::getText()
{
    return text;
}

// void ThinkWidget::toggleWidget()
// {
//     if (!isLabel)
//         return;
//     htmlText.clear();
//     fullHtmlText.clear();

//     int initWidth = int(fontMetrics->horizontalAdvance(text));
//     if (initWidth > maxWidth)
//         webEngineView->setFixedWidth(maxWidth);
//     else {
//         if (text.isEmpty())
//             webEngineView->setFixedSize(20, 66);
//         else
//             webEngineView->setFixedWidth(initWidth);
//     }
//     qDebug() << "webEngineView:" << webEngineView->size();
//     // ---- MathJax 头 ----
//     mathJaxCdn = QString(R"(
// <!DOCTYPE html>
// <html lang="en">
// <head>
// <meta charset="UTF-8">
// <meta name="viewport" content="width=device-width, initial-scale=1.0">
// <script>
// MathJax = {
//   options: { enableMenu: false },
//   tex: { inlineMath:[["$","$"],["\ $","\$"]], displayMath:[["$$","$$"],["\
//  $$","\$$ "]] },
//   svg:  { fontCache: 'global' }
// };
// </script>
// <script src="%1"></script>
// <style>
// table{width:50%;border-collapse:collapse;margin:10px 0;}
// th,td{border:1px solid #000;padding:8px;}
// th{background-color:#b0b0b0;}
// .left-align{text-align:left;}
// .center-align{text-align:center;}
// .right-align{text-align:right;}
// body,html{margin:0;padding:0;width:100%;height:100%;box-sizing:border-box;font-size:%2px;overflow:hidden;}
// .content{width:auto;height:auto;display:flex;flex-direction:column;justify-content:center;}
// </style>
// </head>
// <body>
// <div class="content">
// )")
//                          .arg(mathjaxScriptPath)
//                          .arg(windowFontPixelSize);
//     // ---- markdown → html ----
//     if (!text.isEmpty()) {
//         TableInfo tbl = getTable(text);
//         if (tbl.complete) {
//             QStringList parts = text.split(tbl.tableText);
//             QString before = htmlReplaceText(parts.value(0));
//             QString after = htmlReplaceText(parts.value(1));

//             //            m_htmlText = mistune::markdown(before).toUtf8().constData();
//             MarkdownParser beforeParser;
//             std::vector<MarkdownBlockElement> beforeBlocks;
//             HtmlRenderer beforeHtml;
//             beforeParser.blockParse(before, beforeBlocks);
//             //            beforeHtml.Init();
//             for (size_t i = 0; i < beforeBlocks.size(); i++) {
//                 beforeHtml.blockHtml(beforeBlocks[i]);
//             }
//             //            beforeHtml.Tail();
//             htmlText = beforeHtml.getHtml().toUtf8().constData();

//             htmlText += "<table><thead><tr>";
//             for (int i = 0; i < tbl.col; ++i)
//                 htmlText +=
//                         QString("<th class='%1'>%2</th>")
//                                 .arg(getAlignmentClass(tbl.alignList.value(i)), tbl.items.value(i));
//             htmlText += "</tr></thead><tbody>";
//             for (int r = 1; r < tbl.row; ++r) {
//                 htmlText += "<tr>";
//                 for (int c = 0; c < tbl.col; ++c)
//                     htmlText += QString("<td class='%1'>%2</td>")
//                                         .arg(getAlignmentClass(tbl.alignList.value(c)),
//                                              tbl.items.value(r * tbl.col + c));
//                 htmlText += "</tr>";
//             }
//             htmlText += "</tbody></table>";
//             //            htmlText += mistune::markdown(after).toUtf8().constData();
//             MarkdownParser afterParser;
//             std::vector<MarkdownBlockElement> afterBlocks;
//             HtmlRenderer afterHtml;
//             afterParser.blockParse(after, afterBlocks);
//             //            afterHtml.Init();
//             for (size_t i = 0; i < afterBlocks.size(); i++) {
//                 afterHtml.blockHtml(afterBlocks[i]);
//             }
//             //            afterHtml.Tail();
//             htmlText += afterHtml.getHtml().toUtf8().constData();
//         } else {
//             QString md = htmlReplaceText(text);
//             //            m_htmlText = mistune::markdown(md).toUtf8().constData();
//             MarkdownParser parser;
//             std::vector<MarkdownBlockElement> blocks;
//             HtmlRenderer html;
//             parser.blockParse(md, blocks);
//             //            html.Init();
//             for (size_t i = 0; i < blocks.size(); i++) {
//                 html.blockHtml(blocks[i]);
//             }
//             //            html.Tail();
//             htmlText += html.getHtml().toUtf8().constData();
//         }
//         fullHtmlText = mathJaxCdn + htmlText + "</div></body></html>";
//         // QUrl base = QUrl::fromLocalFile(QFileInfo(".").absolutePath() + "/");
//         QUrl base =
//                 QUrl::fromLocalFile(QFileInfo(QFileInfo(".").absolutePath()).absolutePath() + "/");
//         qDebug() << "base:" << base;

//         mainHLayout->removeWidget(label);
//         label->deleteLater();
//         mainHLayout->addWidget(webEngineView);
//         webEngineView->setHtml(fullHtmlText, base);
//         isLabel = false;
//     } else {
//         mainHLayout->removeWidget(label);
//         label->deleteLater();
//         mainHLayout->addWidget(webEngineView);
//         setFixedSize(webEngineView->size() + QSize(10, 0));
//         emit setSizeFinished();
//         isLabel = false;
//     }
//     qDebug() << "ThinkWidget toggleWidget end";
//     qDebug() << fullHtmlText;
// }

void ThinkWidget::onPageLoadFinished(bool success)
{
    if (success)
        webEngineView->page()->runJavaScript("document.body.style.overflowY='hidden';");
    qDebug() << "onPageLoadFinished" << success << webEngineView->page()->contentsSize() << this;
    // 加载流程已结束（无论成败）：此后若仍量不到正尺寸，视为内容为空而非未就绪
    isPageLoaded = true;
    invalidSizeCount = 0;
    updateSizeTimer->start(20);
}

void ThinkWidget::onContentsSizeChanged(const QSizeF &)
{
    updateSizeTimer->start(20);
    qDebug() << "onContentsSizeChanged" << this;
}

// 量到无效尺寸（w/h <= 0）时是否继续等待：返回 true 表示重启轮询等下次量测
bool ThinkWidget::waitForValidSize()
{
    // 页面加载流程结束前尺寸尚不稳定：继续轮询等待
    if (!isPageLoaded) {
        updateSizeTimer->start(10);
        return true;
    }
    // 页面已加载完成仍量不到正尺寸：空内容块（如流式输出中未闭合的 "**"，
    // 解析后无 HTML 输出）的 .content 高度恒为 0，是合法结果而非"未就绪"。
    // 若无限重试，MessageWidget::setText 的嵌套等待循环永远等不到
    // isSizeFinish，整条消息渲染卡死（39.txt）。限次后按当前尺寸收敛
    invalidSizeCount += 1;
    if (invalidSizeCount < 5) {
        updateSizeTimer->start(10);
        return true;
    }
    invalidSizeCount = 0;
    return false;
}

void ThinkWidget::onUpdateSize()
{
    const char *js = R"(
function getPageSize(){
  var body=document.body, html=document.documentElement;
  var w=Math.max(html.clientWidth,html.scrollWidth,html.offsetWidth);
  var h=Math.max(html.clientHeight,html.scrollHeight,html.offsetHeight);
  var content=document.querySelector('.content');
  if(content){ w=content.offsetWidth; h=content.offsetHeight; }
  return [w,h];
}
getPageSize();
)";
    static int funi = 0;
    // 用 QPointer 持有自身，防止控件在重建中被销毁后异步回调访问悬空指针
    QPointer<ThinkWidget> self = this;
    webEngineView->page()->runJavaScript(js, [self](const QVariant &res) {
        if (!self)
            return;
        if (res.isNull()) {
            if (self->waitForValidSize())
                return;
            // 加载完成后 JS 仍无返回（页面异常）：放弃尺寸等待，标记完成，
            // 避免 MessageWidget 的等待循环死锁；控件保持当前初始宽度
            if (self->isSetTextEnd) {
                self->isSetTextEnd = false;
                self->isSizeFinish = true;
            }
            return;
        }
        QList<QVariant> list = res.toList();
        if (list.size() != 2)
            return;
        int w = list[0].toInt();
        int h = list[1].toInt();
        qDebug() << "WebEngineView get size:" << w << h << self.data();
        if (w <= 0 || h <= 0) {
            if (self->waitForValidSize())
                return;
        }
        w = qMin(w, self->getMaxWidth());
        if (self->webEngineSize == QSize(w, h)) {
            if (self->isSetTextEnd) {
                self->isSetTextEnd = false;
                self->isSizeFinish = true;
            }
            return;
        }
        self->webEngineSize = QSize(w, h);
        funi += 1;
        qDebug() << "funi:" << funi << self.data();
        self->webEngineView->setFixedSize(w, h);
        // mainHLayout->addWidget(webEngineView);
        // if (webEngineView->isHidden()) {
        //     qDebug() << "isHidden:" << webEngineView->isHidden();
        //     webEngineView->show();
        // }
        self->setFixedSize(w + 10, h);
        emit self->setSizeFinished();
        // isEmitSizeFinish = true;
        if (self->isSetTextEnd) {
            self->isSetTextEnd = false;
            self->isSizeFinish = true;
        }
        qDebug() << "ThinkWidget onUpdateSize end" << self.data();
    });
}

// 逐行扫描文本中的全部管道符表格块：每张表格由连续多行「以 | 开头、以 | 结尾」
// 的行组成。原实现把全文首个 | 与末个 | 之间的内容整体当作单张表格解析，
// 多张表格并存时表间的标题、水平线会被并入单元格，行列推导错位导致提取失败；
// 按行分块后各表格独立解析，表格之外的文本仍交由 MarkdownParser 渲染
QVector<ThinkWidget::TableInfo> ThinkWidget::getTables(const QString &text) const
{
    QVector<TableInfo> tables;
    const int n = text.size();
    int pos = 0;
    while (pos < n) {
        const int eol = text.indexOf('\n', pos);
        const int lineEnd = (eol == -1) ? n : eol;
        if (isTableRow(text.mid(pos, lineEnd - pos))) {
            TableInfo t;
            t.start = pos;
            QStringList rows;
            // 收集本表格块的连续表格行，pos 前进到块后的第一行
            while (pos < n) {
                const int e = text.indexOf('\n', pos);
                const int le = (e == -1) ? n : e;
                const QString line = text.mid(pos, le - pos);
                if (!isTableRow(line))
                    break;
                rows << line.trimmed();
                t.end = le;
                pos = (e == -1) ? n : le + 1;
            }
            t.items = splitTableRow(rows.value(0));
            t.col = t.items.size();
            if (rows.size() >= 2)
                t.alignList = splitTableRow(rows.value(1));
            // 结构完整 = 至少「表头 + 对齐行 + 一条数据行」且各行单元格数与表头一致。
            // 流式接收中的半截表格（如仅收到表头）判为不完整，按普通文本渲染，
            // 待补齐后由下一轮 setText 渲染为表格
            bool matched = t.col >= 1 && rows.size() >= 3;
            for (int r = 1; matched && r < rows.size(); ++r) {
                QStringList cells = splitTableRow(rows.value(r));
                if (cells.size() != t.col)
                    matched = false;
                else if (r >= 2)
                    t.items << cells;
            }
            t.row = matched ? rows.size() - 1 : 0;
            t.complete = matched;
            tables << t;
            continue;
        }
        pos = (eol == -1) ? n : eol + 1;
    }
    return tables;
}

// 表格行判定：去掉行首尾空白后以 | 开头、以 | 结尾且至少含两个 |
bool ThinkWidget::isTableRow(const QString &line) const
{
    const QString trim = line.trimmed();
    return trim.startsWith('|') && trim.endsWith('|') && trim.count('|') >= 2;
}

// 表格行按 | 拆分为单元格：行首尾的 | 使 split 结果带首尾两个空片段
QStringList ThinkWidget::splitTableRow(const QString &row) const
{
    QStringList cells = row.split('|');
    cells.removeFirst();
    cells.removeLast();
    return cells;
}

// 拼接全文 HTML：完整的表格块输出为表格，其余文本（含流式接收中的不完整
// 表格块）按 Markdown 解析，与单表格时保持一致的渲染效果
QString ThinkWidget::buildHtmlText(const QString &text) const
{
    QVector<TableInfo> tables = getTables(text);
    QString html;
    int pos = 0;
    for (const TableInfo &tbl : tables) {
        if (!tbl.complete)
            continue;
        html += markdownToHtml(text.mid(pos, tbl.start - pos));
        html += tableToHtml(tbl);
        pos = tbl.end;
    }
    html += markdownToHtml(text.mid(pos));
    return html;
}

QString ThinkWidget::tableToHtml(const TableInfo &tbl) const
{
    QString html = "<table><thead><tr>";
    for (int i = 0; i < tbl.col; ++i)
        html += QString("<th class='%1'>%2</th>")
                        .arg(getAlignmentClass(tbl.alignList.value(i)), tbl.items.value(i));
    html += "</tr></thead><tbody>";
    for (int r = 1; r < tbl.row; ++r) {
        html += "<tr>";
        for (int c = 0; c < tbl.col; ++c)
            html += QString("<td class='%1'>%2</td>")
                            .arg(getAlignmentClass(tbl.alignList.value(c)),
                                 tbl.items.value(r * tbl.col + c));
        html += "</tr>";
    }
    html += "</tbody></table>";
    return html;
}

QString ThinkWidget::markdownToHtml(const QString &text) const
{
    QString md = htmlReplaceText(text);
    MarkdownParser parser;
    std::vector<MarkdownBlockElement> blocks;
    HtmlRenderer html;
    parser.blockParse(md, blocks);
    for (size_t i = 0; i < blocks.size(); i++) {
        html.blockHtml(blocks[i]);
    }
    return html.getHtml().toUtf8().constData();
}

QString ThinkWidget::getAlignmentClass(const QString &fmt) const
{
    if (fmt.contains(":-") && fmt.contains("-:"))
        return "center-align";
    if (fmt.contains(":-"))
        return "left-align";
    if (fmt.contains("-:"))
        return "right-align";
    return "";
}

QString ThinkWidget::htmlReplaceText(const QString &text) const
{
    QString s = text;
    // s.replace("\\$", "\\\\$");
    s.replace("\frac", "\\frac");
    // s.replace("\\,", "\\\\,");
    s.replace("\alpha", "\\alpha");
    s.replace("\beta", "\\beta");
    s.replace("\theta", "\\theta");
    s.replace("\nu", "\\nu");
    s.replace("\rho", "\\rho");
    s.replace("\tau", "\\tau");
    return s;
}

bool ThinkWidget::hasSelectedText() const
{
    return webEngineView->hasSelection();
}

QString ThinkWidget::getSelectedText() const
{
    return webEngineView->selectedText();
}

// void ThinkWidget::setIsEmitSizeFinish(bool flag)
// {
//     isEmitSizeFinish = flag;
// }

// bool ThinkWidget::getIsEmitSizeFinish()
// {
//     return isEmitSizeFinish;
// }

void ThinkWidget::setIsSizeFinish(bool flag)
{
    isSizeFinish = flag;
}

bool ThinkWidget::getIsSizeFinish()
{
    return isSizeFinish;
}
