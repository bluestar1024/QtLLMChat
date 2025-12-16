#include "textshow.h"

#include <QDebug>

#include <cmath>

TextShow::TextShow(const QString &text, bool isUser, int maxWidth, QWidget *parent)
    : QWidget(parent), text(text.trimmed()), isUser(isUser), maxWidth(maxWidth - 10)
{
    int fontId = QFontDatabase::addApplicationFont(fontFilePath);
    bool hasFontVar = false;
    if (fontId != -1) {
        QStringList families = QFontDatabase::applicationFontFamilies(fontId);
        if (!families.isEmpty()) {
            font = QFont(families.first());
            font.setPixelSize(windowFontPixelSize);
            hasFontVar = true;
        }
    }
    if (hasFontVar)
        fontMetrics = new QFontMetricsF(font);

    label = new CustomLabel();
    label->setTextInteractionFlags(Qt::TextSelectableByMouse);
    label->setWordWrap(true);
    label->setMaximumWidth(this->maxWidth);
    if (hasFontVar)
        label->setFont(font);

    webEngineView = new WebEngineView();
    webEngineView->setMaximumWidth(this->maxWidth);
    connect(webEngineView->page(), &QWebEnginePage::loadFinished, this,
            &TextShow::onPageLoadFinished);
    connect(webEngineView->page(), &QWebEnginePage::contentsSizeChanged, this,
            &TextShow::onContentsSizeChanged);

    isLabel = true;
    mainHLayout = new QHBoxLayout(this);
    mainHLayout->addWidget(label);
    mainHLayout->setContentsMargins(5, 0, 5, 0);
    setText(this->text);

    updateSizeTimer = new QTimer(this);
    updateSizeTimer->setSingleShot(true);
    connect(updateSizeTimer, &QTimer::timeout, this, &TextShow::onUpdateSize);
    firstExecuteNextEmit = true;
    qDebug() << isUser << "TextShow init end";
}

TextShow::~TextShow() { }

void TextShow::setText(const QString &text)
{
    this->text = text.trimmed();
    int labelWidth = 0, labelHeight = 0;
    if (!this->text.isEmpty()) {
        measureText(this->text, labelWidth, labelHeight);
        label->setText(this->text);
        label->setFixedSize(labelWidth, labelHeight);
        setFixedSize(labelWidth + 10, labelHeight);
    } else {
        int h = int(fontMetrics->height());
        label->setFixedSize(h, h);
        setFixedSize(label->size() + QSize(10, 0));
    }
}

void TextShow::measureText(const QString &text, int &labelWidth, int &labelHeight) const
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

void TextShow::toggleWidget()
{
    if (!isLabel)
        return;
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
body,html{margin:0;padding:0;width:100%;height:100%;box-sizing:border-box;font-size:%2px;}
.content{width:auto;height:auto;display:flex;flex-direction:column;justify-content:center;}
</style>
</head>
<body>
<div class="content">
)")
                         .arg(mathjaxScriptPath, windowFontPixelSize);
    // ---- markdown → html ----
    if (!text.isEmpty()) {
        TableInfo tbl = getTable(text);
        if (tbl.complete) {
            QStringList parts = text.split(tbl.tableText);
            QString before = htmlReplaceText(parts.value(0));
            QString after = htmlReplaceText(parts.value(1));

            //            m_htmlText = mistune::markdown(before.toStdString()).c_str();
            MarkdownParser beforeParser;
            std::vector<MarkdownBlockElement> beforeBlocks;
            HtmlRenderer beforeHtml;
            beforeParser.blockParse(before.toStdString(), beforeBlocks);
            //            beforeHtml.Init();
            for (size_t i = 0; i < beforeBlocks.size(); i++) {
                beforeHtml.blockHtml(beforeBlocks[i]);
            }
            //            beforeHtml.Tail();
            htmlText = beforeHtml.getHtml().c_str();

            htmlText += "<table><thead><tr>";
            for (int i = 0; i < tbl.col; ++i)
                htmlText +=
                        QString("<th class='%1'>%2</th>")
                                .arg(getAlignmentClass(tbl.alignList.value(i)), tbl.items.value(i));
            htmlText += "</tr></thead><tbody>";
            for (int r = 1; r < tbl.row; ++r) {
                htmlText += "<tr>";
                for (int c = 0; c < tbl.col; ++c)
                    htmlText += QString("<td class='%1'>%2</td>")
                                        .arg(getAlignmentClass(tbl.alignList.value(c)),
                                             tbl.items.value(r * tbl.col + c));
                htmlText += "</tr>";
            }
            htmlText += "</tbody></table>";
            //            htmlText += mistune::markdown(after.toStdString()).c_str();
            MarkdownParser afterParser;
            std::vector<MarkdownBlockElement> afterBlocks;
            HtmlRenderer afterHtml;
            afterParser.blockParse(after.toStdString(), afterBlocks);
            //            afterHtml.Init();
            for (size_t i = 0; i < afterBlocks.size(); i++) {
                afterHtml.blockHtml(afterBlocks[i]);
            }
            //            afterHtml.Tail();
            htmlText += afterHtml.getHtml().c_str();
        } else {
            QString md = htmlReplaceText(text);
            //            m_htmlText = mistune::markdown(md.toStdString()).c_str();
            MarkdownParser parser;
            std::vector<MarkdownBlockElement> blocks;
            HtmlRenderer html;
            parser.blockParse(md.toStdString(), blocks);
            //            html.Init();
            for (size_t i = 0; i < blocks.size(); i++) {
                html.blockHtml(blocks[i]);
            }
            //            html.Tail();
            htmlText += html.getHtml().c_str();
        }
        fullHtmlText = mathJaxCdn + htmlText + "</div></body></html>";
        // QUrl base = QUrl::fromLocalFile(QFileInfo(".").absolutePath() + "/");
        QUrl base =
                QUrl::fromLocalFile(QFileInfo(QFileInfo(".").absolutePath()).absolutePath() + "/");
        qDebug() << "base:" << base;

        mainHLayout->removeWidget(label);
        label->deleteLater();
        mainHLayout->addWidget(webEngineView);
        webEngineView->setHtml(fullHtmlText, base);
        isLabel = false;
    } else {
        mainHLayout->removeWidget(label);
        label->deleteLater();
        mainHLayout->addWidget(webEngineView);
        setFixedSize(webEngineView->size() + QSize(10, 0));
        emit setSizeFinished();
        isLabel = false;
    }
    qDebug() << isUser << "TextShow toggleWidget end";
    qDebug() << fullHtmlText;
}

void TextShow::onPageLoadFinished(bool success)
{
    if (success)
        webEngineView->page()->runJavaScript("document.body.style.overflowY='hidden';");
    qDebug() << isUser << "onPageLoadFinished";
}

void TextShow::onContentsSizeChanged(const QSizeF &)
{
    updateSizeTimer->start(20);
    qDebug() << isUser << "onContentsSizeChanged";
}

void TextShow::onUpdateSize()
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
    webEngineView->page()->runJavaScript(js, [this](const QVariant &res) {
        if (res.isNull()) {
            updateSizeTimer->start(10);
            return;
        }
        QList<QVariant> list = res.toList();
        if (list.size() != 2)
            return;
        int w = list[0].toInt();
        int h = list[1].toInt();
        qDebug() << "WebEngineView get size:" << w << h;
        if (w <= 0 || h <= 0)
            return;
        webEngineView->setFixedSize(w, h);
        setFixedSize(w + 10, h);
        emit setSizeFinished();
        if (firstExecuteNextEmit) {
            firstExecuteNextEmit = false;
            emit executeNext();
            qDebug() << "TextShow executeNext emit";
        }
        qDebug() << isUser << "TextShow onUpdateSize end";
    });
}

TextShow::TableInfo TextShow::getTable(const QString &text) const
{
    TableInfo t;
    int firstPipe = text.indexOf('|');
    if (firstPipe == -1)
        return t;
    int lastPipe = text.lastIndexOf('|');
    if (lastPipe == -1 || lastPipe <= firstPipe)
        return t;

    t.tableText = text.mid(firstPipe, lastPipe - firstPipe + 1);
    QStringList segs = t.tableText.split('|');
    segs.removeFirst();
    segs.removeLast();
    int row = 0, rowFull = 0, col = 0;
    for (int i = 0; i < segs.size(); i++) {
        if (segs[i].contains('\n'))
            ++rowFull;
        else {
            if (rowFull == 0)
                col += 1;
            if (i == segs.size() - 1)
                ++rowFull;
        }
    }
    if (rowFull > 1)
        row = rowFull - 1;
    else
        row = rowFull;
    if (row >= 1)
        t.items << segs.mid(0, col);
    if (rowFull >= 2)
        t.alignList << segs.mid(col + 1, col);
    if (row >= 2) {
        for (int r = 1; r < row; r++)
            t.items << segs.mid((r + 1) * (col + 1), col);
        if (segs.size() == rowFull * (col + 1) - 1)
            t.complete = true;
    }
    t.row = row;
    t.col = col;
    return t;
}

QString TextShow::getAlignmentClass(const QString &fmt) const
{
    if (fmt.contains(":-") && fmt.contains("-:"))
        return "center-align";
    if (fmt.contains(":-"))
        return "left-align";
    if (fmt.contains("-:"))
        return "right-align";
    return "";
}

QString TextShow::htmlReplaceText(const QString &text) const
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

bool TextShow::hasSelectedText() const
{
    return isLabel ? label->hasSelectedText() : webEngineView->hasSelection();
}

QString TextShow::selectedText() const
{
    return isLabel ? label->selectedText() : webEngineView->selectedText();
}
