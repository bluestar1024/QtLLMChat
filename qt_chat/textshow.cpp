#include "textshow.h"
#include <cmath>
#include <QDebug>

TextShow::TextShow(const QString &text, bool isUser, int maxWidth, QWidget *parent)
    : QWidget(parent),
      m_text(text.trimmed()),
      m_isUser(isUser),
      m_maxWidth(maxWidth - 10)
{
    int fontId = QFontDatabase::addApplicationFont(fontFilePath);
    if (fontId != -1) {
        QStringList families = QFontDatabase::applicationFontFamilies(fontId);
        if (!families.isEmpty()) {
            m_font = QFont(families.first());
            m_font.setPixelSize(windowFontPixelSize);
        }
    }
    m_fontMetrics = new QFontMetricsF(m_font);

    m_label = new CustomLabel();
    m_label->setTextInteractionFlags(Qt::TextSelectableByMouse);
    m_label->setWordWrap(true);
    m_label->setMaximumWidth(m_maxWidth);
    m_label->setFont(m_font);

    m_webEngineView = new WebEngineView();
    m_webEngineView->setMaximumWidth(m_maxWidth);
    connect(m_webEngineView->page(), &QWebEnginePage::loadFinished,
            this, &TextShow::onPageLoadFinished);
    connect(m_webEngineView->page(), &QWebEnginePage::contentsSizeChanged,
            this, &TextShow::onContentsSizeChanged);

    m_isLabel = true;
    m_mainHLayout = new QHBoxLayout(this);
    m_mainHLayout->addWidget(m_label);
    m_mainHLayout->setContentsMargins(5, 0, 5, 0);
    setText(m_text);

    m_updateSizeTimer = new QTimer(this);
    m_updateSizeTimer->setSingleShot(true);
    connect(m_updateSizeTimer, &QTimer::timeout, this, &TextShow::onUpdateSize);
    m_firstExecuteNextEmit = true;
    qDebug() << m_isUser <<"TextShow init end";
}

TextShow::~TextShow()
{
    delete m_fontMetrics;
}

void TextShow::setText(const QString &text)
{
    m_text = text.trimmed();
    int labelWidth = 0, labelHeight = 0;
    if (!m_text.isEmpty()) {
        measureText(m_text, labelWidth, labelHeight);
        m_label->setText(m_text);
        m_label->setFixedSize(labelWidth, labelHeight);
        setFixedSize(labelWidth + 10, labelHeight);
    } else {
        int h = int(m_fontMetrics->height());
        m_label->setFixedSize(h, h);
        setFixedSize(m_label->size() + QSize(10, 0));
    }
}

void TextShow::measureText(const QString &text,
                           int &labelWidth,
                           int &labelHeight) const
{
    int textHeight = int(m_fontMetrics->height());
    QStringList lines = text.split('\n');
    int maxLineWidth = 0;
    for (const QString &line : lines)
        maxLineWidth = qMax(maxLineWidth, int(m_fontMetrics->horizontalAdvance(line)));

    if (maxLineWidth + 4 < m_maxWidth) {
        labelWidth  = maxLineWidth + 4;
        labelHeight = lines.size() * (textHeight + 3) - 3;
    } else {
        int totalWidth = 0;
        for (int i = 0; i < lines.size(); ++i) {
            qreal w = m_fontMetrics->horizontalAdvance(lines[i] + (i < lines.size() - 1 ? " " : ""));
            totalWidth += std::ceil(w / (m_maxWidth - 24)) * (m_maxWidth - 24);
        }
        labelWidth  = m_maxWidth;
        labelHeight = std::ceil(totalWidth / qreal(m_maxWidth - 24)) * (textHeight + 3) - 3;
    }
}

void TextShow::toggleWidget()
{
    if (!m_isLabel) return;
    m_htmlText.clear();
    m_fullHtmlText.clear();

    int initWidth = int(m_fontMetrics->horizontalAdvance(m_text));
    if (initWidth > m_maxWidth)
        m_webEngineView->setFixedWidth(m_maxWidth);
    else {
        if (m_text.isEmpty())
            m_webEngineView->setFixedSize(20, 66);
        else
            m_webEngineView->setFixedWidth(initWidth);
    }
    // ---- MathJax 头 ----
    m_mathJaxCdn = QString(R"(
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
    if (!m_text.isEmpty()) {
        TableInfo tbl = getTable(m_text);
        if (tbl.complete) {
            QStringList parts = m_text.split(tbl.tableText);
            QString before = htmlReplaceText(parts.value(0));
            QString after  = htmlReplaceText(parts.value(1));

//            m_htmlText = mistune::markdown(before.toStdString()).c_str();
            Markdown_Parser before_parser;
            std::vector<Markdown_BlockElement> before_blocks;
            Html_Renderer before_html;
            before_parser.block_parse(before.toStdString(), before_blocks);
//            before_html.Init();
            for (size_t i = 0; i < before_blocks.size(); i++) {
                before_html.BlockHtml(before_blocks[i]);
            }
//            before_html.Tail();
            m_htmlText = before_html.getHtml().c_str();

            m_htmlText += "<table><thead><tr>";
            for (int i = 0; i < tbl.col; ++i)
                m_htmlText += QString("<th class='%1'>%2</th>")
                                  .arg(getAlignmentClass(tbl.alignList.value(i)), tbl.items.value(i));
            m_htmlText += "</tr></thead><tbody>";
            for (int r = 1; r < tbl.row; ++r) {
                m_htmlText += "<tr>";
                for (int c = 0; c < tbl.col; ++c)
                    m_htmlText += QString("<td class='%1'>%2</td>")
                                      .arg(getAlignmentClass(tbl.alignList.value(c)), tbl.items.value(r * tbl.col + c));
                m_htmlText += "</tr>";
            }
            m_htmlText += "</tbody></table>";
//            m_htmlText += mistune::markdown(after.toStdString()).c_str();
            Markdown_Parser after_parser;
            std::vector<Markdown_BlockElement> after_blocks;
            Html_Renderer after_html;
            after_parser.block_parse(after.toStdString(), after_blocks);
//            after_html.Init();
            for (size_t i = 0; i < after_blocks.size(); i++) {
                after_html.BlockHtml(after_blocks[i]);
            }
//            after_html.Tail();
            m_htmlText += after_html.getHtml().c_str();
        } else {
            QString md = htmlReplaceText(m_text);
//            m_htmlText = mistune::markdown(md.toStdString()).c_str();
            Markdown_Parser parser;
            std::vector<Markdown_BlockElement> blocks;
            Html_Renderer html;
            parser.block_parse(md.toStdString(), blocks);
//            html.Init();
            for (size_t i = 0; i < blocks.size(); i++) {
                html.BlockHtml(blocks[i]);
            }
//            html.Tail();
            m_htmlText += html.getHtml().c_str();
        }
        m_fullHtmlText = m_mathJaxCdn + m_htmlText + "</div></body></html>";
        // QUrl base = QUrl::fromLocalFile(QFileInfo(".").absolutePath() + "/");
        QUrl base = QUrl::fromLocalFile(QFileInfo(QFileInfo(".").absolutePath()).absolutePath() + "/");
        qDebug() << "base:" << base;

        m_mainHLayout->removeWidget(m_label);
        m_label->deleteLater();
        m_mainHLayout->addWidget(m_webEngineView);
        m_webEngineView->setHtml(m_fullHtmlText, base);
        m_isLabel = false;
    }
    else {
        m_mainHLayout->removeWidget(m_label);
        m_label->deleteLater();
        m_mainHLayout->addWidget(m_webEngineView);
        setFixedSize(m_webEngineView->size() + QSize(10, 0));
        emit setSizeFinished();
        m_isLabel = false;
    }
    qDebug() << m_isUser <<"TextShow toggleWidget end";
    qDebug() << m_fullHtmlText;
}

void TextShow::onPageLoadFinished(bool success)
{
    if (success)
        m_webEngineView->page()->runJavaScript("document.body.style.overflowY='hidden';");
    qDebug() << m_isUser << "onPageLoadFinished";
}

void TextShow::onContentsSizeChanged(const QSizeF &)
{
    m_updateSizeTimer->start(20);
    qDebug() << m_isUser << "onContentsSizeChanged";
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
    m_webEngineView->page()->runJavaScript(js, [this](const QVariant &res) {
        if (res.isNull()) {
            m_updateSizeTimer->start(10);
            return;
        }
        QList<QVariant> list = res.toList();
        if (list.size() != 2) return;
        int w = list[0].toInt();
        int h = list[1].toInt();
        qDebug() << "WebEngineView get size:" << w << h;
        if (w <= 0 || h <= 0) return;
        m_webEngineView->setFixedSize(w, h);
        setFixedSize(w + 10, h);
        emit setSizeFinished();
        if (m_firstExecuteNextEmit) {
            m_firstExecuteNextEmit = false;
            emit executeNext();
            qDebug() << "TextShow executeNext emit";
        }
        qDebug() << m_isUser << "TextShow onUpdateSize end";
    });
}

TextShow::TableInfo TextShow::getTable(const QString &text) const
{
    TableInfo t;
    int firstPipe = text.indexOf('|');
    if (firstPipe == -1) return t;
    int lastPipe = text.lastIndexOf('|');
    if (lastPipe == -1 || lastPipe <= firstPipe) return t;

    t.tableText = text.mid(firstPipe, lastPipe - firstPipe + 1);
    QStringList segs = t.tableText.split('|');
    segs.removeFirst();
    segs.removeLast();
    int row = 0, rowFull = 0, col = 0;
    for (int i = 0; i < segs.size(); i++) {
        if (segs[i].contains('\n')) ++rowFull;
        else {
            if (rowFull == 0) col += 1;
            if (i == segs.size() - 1) ++rowFull;
        }
    }
    if (rowFull > 1) row = rowFull - 1;
    else row = rowFull;
    if (row >= 1) t.items << segs.mid(0, col);
    if (rowFull >= 2) t.alignList << segs.mid(col + 1, col);
    if (row >= 2) {
        for(int r = 1; r < row; r++) t.items << segs.mid((r + 1) * (col + 1), col);
        if (segs.size() == rowFull * (col + 1) - 1) t.complete = true;
    }
    t.row = row;
    t.col = col;
    return t;
}

QString TextShow::getAlignmentClass(const QString &fmt) const
{
    if (fmt.contains(":-") && fmt.contains("-:")) return "center-align";
    if (fmt.contains(":-")) return "left-align";
    if (fmt.contains("-:")) return "right-align";
    return "";
}

QString TextShow::htmlReplaceText(const QString &text) const
{
    QString s = text;
    // s.replace("\\$", "\\\\$");
    s.replace("\frac", "\\frac");
    // s.replace("\\,", "\\\\,");
    s.replace("\alpha", "\\alpha");
    s.replace("\beta",  "\\beta");
    s.replace("\theta", "\\theta");
    s.replace("\nu",    "\\nu");
    s.replace("\rho",   "\\rho");
    s.replace("\tau",   "\\tau");
    return s;
}

bool TextShow::hasSelectedText() const
{
    return m_isLabel ? m_label->hasSelectedText()
                     : m_webEngineView->hasSelection();
}

QString TextShow::selectedText() const
{
    return m_isLabel ? m_label->selectedText()
                     : m_webEngineView->selectedText();
}
