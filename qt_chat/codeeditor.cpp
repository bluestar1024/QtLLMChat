#include "codeeditor.h"
#include "appcontext.h"

#include <QtGui/QFontMetrics>
#include <QtGui/QFontInfo>
#include <QtGui/QTextBlock>
#include <QtGui/QTextDocument>
#include <QtGui/QPalette>
#include <QtGui/QAbstractTextDocumentLayout>

CodeEditor::CodeEditor(AppContext *appContext, int maxWidth, QWidget *parent)
    : QTextEdit(parent), appContext(appContext), text(""), lexerName(""), isResetText(false),
      isDestroying(false), highlighter(nullptr)
{
    // qDebug() << "CodeEditor start" << this;
    setFixedWidth(maxWidth);
    horizontalScrollBar()->setCursor(Qt::PointingHandCursor);

    // qDebug() << "CodeEditor ing0" << this;
    applyDpiFont();

    // qDebug() << "CodeEditor ing1" << this;
    lineNumberArea = new LineNumberArea(this);
    lineNumberArea->move(0, 0);
    updateLineNumberAreaWidth();

    // qDebug() << "CodeEditor ing4" << this;
    setThemeStyle(false);

    // qDebug() << "CodeEditor ing2" << this;
    highlighters.insert("None", nullptr);
    highlighters.insert("Python", new PythonHighlighter());
    highlighters.insert("C++", new CXXHighlighter());
    highlighters.insert("GLSL", new GLSLHighlighter());
    highlighters.insert("LUA", new LuaHighlighter());
    highlighter = highlighters["None"];

    // qDebug() << "CodeEditor ing3" << this;
    connect(this, &QTextEdit::textChanged, this, &CodeEditor::adjustHeight);
    connect(verticalScrollBar(), &QScrollBar::valueChanged, lineNumberArea,
            QOverload<>::of(&QWidget::update));
    // qDebug() << "CodeEditor end" << this;
}

CodeEditor::~CodeEditor()
{
    // 析构期间先断开自身连接：高亮器等子对象析构可能触发 textChanged →
    // adjustHeight → emit setSizeFinished，向已销毁的外层控件传播回调导致悬空访问
    disconnect(this, &QTextEdit::textChanged, this, &CodeEditor::adjustHeight);
    isDestroying = true;
    qDeleteAll(highlighters);
}

void CodeEditor::setThemeStyle(bool light)
{
    // qDebug() << "CodeEditor setThemeStyle start" << this;
    if (light) {
        // qDebug() << "CodeEditor setThemeStyle ing0" << this;
        setStyleSheet(
                "QTextEdit{ border:none; background:#ffffff;"
                "border-bottom-left-radius:7px; border-bottom-right-radius:7px; }"
                "QScrollBar:horizontal{ background:transparent; height:10px; padding:0 0 4px 0; }"
                "QScrollBar::handle:horizontal{ background:#bcbcb4; height:6px; border-radius:3px; "
                "}"
                "QScrollBar::handle:horizontal:hover{ background:#8c8c8c; }"
                "QScrollBar::add-page:horizontal, QScrollBar::sub-page:horizontal{ "
                "background:transparent; }"
                "QScrollBar::add-line:horizontal, QScrollBar::sub-line:horizontal{ width:0px; }");
        QPalette p = palette();
        // qDebug() << "CodeEditor setThemeStyle ing1" << this;
        p.setColor(QPalette::Text, QColor(78, 86, 92));
        setPalette(p);
        lineNumberArea->setLightTheme();
        appContext->setCodeThemeFilePath(":/config/light_theme.xml");
        // qDebug() << "CodeEditor setThemeStyle ing2" << this;
    } else {
        // qDebug() << "CodeEditor setThemeStyle ing3" << this;
        setStyleSheet(
                "QTextEdit{ border:none; background:#14141c;"
                "border-bottom-left-radius:7px; border-bottom-right-radius:7px; }"
                "QScrollBar:horizontal{ background:transparent; height:10px; padding:0 0 4px 0; }"
                "QScrollBar::handle:horizontal{ background:#44444c; height:6px; border-radius:3px; "
                "}"
                "QScrollBar::handle:horizontal:hover{ background:#747474; }"
                "QScrollBar::add-page:horizontal, QScrollBar::sub-page:horizontal{ "
                "background:transparent; }"
                "QScrollBar::add-line:horizontal, QScrollBar::sub-line:horizontal{ width:0px; }");
        QPalette p = palette();
        // qDebug() << "CodeEditor setThemeStyle ing4" << this;
        p.setColor(QPalette::Text, QColor(178, 170, 164));
        setPalette(p);
        // qDebug() << "CodeEditor setThemeStyle ing6" << this;
        lineNumberArea->setDarkTheme();
        // qDebug() << "CodeEditor setThemeStyle ing7" << this;
        appContext->setCodeThemeFilePath(":/config/dark_theme.xml");
        // qDebug() << "CodeEditor setThemeStyle ing5" << this;
    }
    highlightCode(text, lexerName);
    // qDebug() << "CodeEditor setThemeStyle end" << this;
}

int CodeEditor::getFirstVisibleBlockNumber() const
{
    QTextCursor curs(document());
    curs.movePosition(QTextCursor::Start);
    QRect viewportRect = viewport()->geometry();

    for (int i = 0; i < document()->blockCount(); ++i) {
        QTextBlock blk = curs.block();
        QRect br = document()
                           ->documentLayout()
                           ->blockBoundingRect(blk)
                           .translated(viewport()->geometry().x(),
                                       viewport()->geometry().y()
                                               - verticalScrollBar()->sliderPosition())
                           .toRect();
        if (viewportRect.intersects(br))
            return i;
        curs.movePosition(QTextCursor::NextBlock);
    }
    return 0;
}

void CodeEditor::updateLineNumberAreaWidth()
{
    int digits = QString::number(document()->blockCount()).length();
    int width = fontMetrics().horizontalAdvance('9') * (digits + 1);
    lineNumberArea->setFixedSize(width, viewport()->height());
    setViewportMargins(width, 0, 0, 0);
}

void CodeEditor::applyDpiFont()
{
    // 字体选择：优先 appContext->fontFamily() 且实际渲染为等宽字体；
    // 字体族不是等宽字体时（如微软雅黑为比例字体，setFixedPitch 只是请求
    // 并不能把字形变为等宽）回退系统等宽字体——CodeEditor 展示代码必须
    // 等宽，允许字体族例外不满足全局统一
    QFont font;
    const QString &fontFamily = this->appContext->fontFamily();
    if (!fontFamily.isEmpty()) {
        QFont candidate(fontFamily);
        candidate.setFixedPitch(true);
        // QFontInfo 查询实际匹配字体的等宽属性，避免“请求等宽但渲染比例”的偏差
        if (QFontInfo(candidate).fixedPitch())
            font = candidate;
    }
    if (font.family().isEmpty() || !QFontInfo(font).fixedPitch())
        font = QFontDatabase::systemFont(QFontDatabase::FixedFont);
    font.setFixedPitch(true);
    // 像素字号与全局一致：pointSize 会在每个屏的 DPI 下被 Qt 自动重解析
    // （跨屏后物理尺寸不变、设备像素数变化），而 WebEngineView 的 HTML
    // 使用写死的像素字号，导致跨屏后两者字号不一致（Bug 2）；统一改用
    // 像素字号后由 applyScreenDpiChange 驱动的重建链负责跨屏缩放
    font.setPixelSize(this->appContext->windowFontPixelSize());
    setFont(font);
}

void CodeEditor::updateSize(qreal curDpi, qreal lastDpi)
{
    if (lastDpi <= 0 || curDpi <= 0)
        return;
    applyDpiFont();
    // 宽度由 CodeShow 按新 DPI 显式设置（与构造时一致：CodeShow 宽 - 2），
    // 这里只重算高度：文档按新字号重新排版后 adjustHeight 会把控件高度改为
    // 正确值，并通过 setSizeFinished 通知 CodeShow 重算自身高度
    adjustHeight();
}

void CodeEditor::adjustHeight()
{
    if (isDestroying)
        return;
    qDebug() << "CodeEditor adjustHeight start" << this;
    qDebug() << "CodeEditor adjustHeight docH" << document()->size().height()
             << "fontPixel" << font().pixelSize() << "blocks" << document()->blockCount()
             << "selfH" << height() << "fixedW" << width();
    int h = int(document()->size().height()) + 15;
    setFixedHeight(h);
    updateLineNumberAreaWidth();
    lineNumberArea->update();
    emit setSizeFinished();
    qDebug() << "CodeEditor adjustHeight end" << this;
}

void CodeEditor::resizeEvent(QResizeEvent *e)
{
    qDebug() << "CodeEditor resizeEvent start" << this;
    QTextEdit::resizeEvent(e);
    if (isDestroying)
        return;
    if (height() != int(document()->size().height()) + 15)
        adjustHeight();
    qDebug() << "CodeEditor resizeEvent end" << this;
}

void CodeEditor::highlightCode(const QString &text, const QString &lexer)
{
    qDebug() << "CodeEditor highlightCode start" << this;
    QString appendText = text;
    if (!this->text.isEmpty() && text.contains(this->text))
        appendText = text.mid(this->text.length());
    if (appendText == text && this->text.contains(text))
        isResetText = true;
    this->text = text;
    lexerName = lexer;

    qDebug() << "CodeEditor highlightCode ing0" << this;
    StyleSyntaxHighlighter *next = nullptr;
    if (lexer == "cpp")
        next = highlighters["C++"];
    else if (lexer == "python")
        next = highlighters["Python"];
    else if (lexer == "glsl")
        next = highlighters["GLSL"];
    else if (lexer == "lua")
        next = highlighters["LUA"];
    else if (lexer.isEmpty())
        // 普通文本段（无语言标识）：不做语法高亮，正文颜色与代码段一致
        next = highlighters["None"];
    else
        next = highlighters["C++"];
    setHighlighter(next);

    qDebug() << "CodeEditor highlightCode ing1" << this;
    qDebug() << "text:" << text << this;
    qDebug() << "lexer:" << lexer << this;
    qDebug() << "isResetText:" << isResetText << this;
    if (!isResetText) {
        qDebug() << "CodeEditor highlightCode ing2" << this;
        setUpdatesEnabled(false);
        qDebug() << "CodeEditor highlightCode ing6" << this;
        QTextCursor c(document());
        qDebug() << "CodeEditor highlightCode ing7" << this;
        c.movePosition(QTextCursor::End);
        c.beginEditBlock();
        c.insertText(appendText);
        qDebug() << "CodeEditor highlightCode ing8" << this;
        c.endEditBlock();
        qDebug() << "CodeEditor highlightCode ing9" << this;
        setUpdatesEnabled(true);
        qDebug() << "CodeEditor highlightCode ing3" << this;
    } else {
        qDebug() << "CodeEditor highlightCode ing4" << this;
        setUpdatesEnabled(false);
        setPlainText(this->text);
        setUpdatesEnabled(true);
        isResetText = false;
        qDebug() << "CodeEditor highlightCode ing5" << this;
    }
    qDebug() << "CodeEditor highlightCode end" << this;
}

void CodeEditor::setHighlighter(StyleSyntaxHighlighter *high)
{
    if (highlighter != high) {
        // 旧高亮器需从文档摘除：切到普通文本段（None）后，若旧高亮器仍附着在
        // 文档上，会继续按旧语言给新插入的文本着色
        if (highlighter && highlighter->document() == document())
            highlighter->setDocument(nullptr);
        highlighter = high;
    }
    if (highlighter && highlighter->document() != document())
        highlighter->setDocument(document());
    if (highlighter)
        highlighter->setSyntaxStyle(SyntaxStyle::defaultStyle(appContext));
}
