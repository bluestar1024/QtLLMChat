#include "codeeditor.h"

#include <QtGui/QFontMetrics>
#include <QtGui/QTextBlock>
#include <QtGui/QTextDocument>
#include <QtGui/QPalette>
#include <QtGui/QAbstractTextDocumentLayout>

CodeEditor::CodeEditor(int maxWidth, QWidget *parent)
    : QTextEdit(parent), text(""), lexerName(""), isResetText(false)
{
    setFixedWidth(maxWidth);
    horizontalScrollBar()->setCursor(Qt::PointingHandCursor);

    setThemeStyle(false);

    QFont font = QFontDatabase::systemFont(QFontDatabase::FixedFont);
    font.setFixedPitch(true);
    font.setPointSize(windowFontPointSize);
    setFont(font);

    lineNumberArea = new LineNumberArea(this);
    lineNumberArea->move(0, 0);
    updateLineNumberAreaWidth();

    highlighters.insert("None", nullptr);
    highlighters.insert("Python", new QPythonHighlighter(this));
    highlighters.insert("C++", new QCXXHighlighter(this));
    highlighters.insert("GLSL", new QGLSLHighlighter(this));
    highlighters.insert("LUA", new QLuaHighlighter(this));
    highlighter = highlighters["None"];

    connect(this, &QTextEdit::textChanged, this, &CodeEditor::adjustHeight);
    connect(verticalScrollBar(), &QScrollBar::valueChanged, lineNumberArea,
            QOverload<>::of(&QWidget::update));
}

CodeEditor::~CodeEditor()
{
    qDeleteAll(highlighters);
}

void CodeEditor::setThemeStyle(bool light)
{
    if (light) {
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
        p.setColor(QPalette::Text, QColor(78, 86, 92));
        setPalette(p);
        lineNumberArea->setLightTheme();
        codeThemeFilePath = ":/config/light_theme.xml";
    } else {
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
        p.setColor(QPalette::Text, QColor(178, 170, 164));
        setPalette(p);
        lineNumberArea->setDarkTheme();
        codeThemeFilePath = ":/config/dark_theme.xml";
    }
    highlightCode(text, lexerName);
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

void CodeEditor::adjustHeight()
{
    int h = int(document()->size().height()) + 15;
    setFixedHeight(h);
    updateLineNumberAreaWidth();
    lineNumberArea->update();
    emit setSizeFinished();
}

void CodeEditor::resizeEvent(QResizeEvent *e)
{
    QTextEdit::resizeEvent(e);
    if (height() != int(document().size().height()) + 15)
        adjustHeight();
}

void CodeEditor::highlightCode(const QString &text, const QString &lexer)
{
    QString appendText = text;
    if (!this->text.isEmpty() && text.contains(this->text))
        appendText = text.mid(this->text.length());
    if (appendText == text && this->text.contains(text))
        isResetText = true;
    this->text = text;
    lexerName = lexer;

    QSyntaxHighlighter *next = nullptr;
    if (lexer == "cpp")
        next = highlighters["C++"];
    else if (lexer == "python")
        next = highlighters["Python"];
    else if (lexer == "glsl")
        next = highlighters["GLSL"];
    else if (lexer == "lua")
        next = highlighters["LUA"];
    else
        next = highlighters["C++"];
    setHighlighter(next);

    if (!isResetText) {
        setUpdatesEnabled(false);
        QTextCursor c(document());
        c.movePosition(QTextCursor::End);
        c.beginEditBlock();
        c.insertText(appendText);
        c.endEditBlock();
        setUpdatesEnabled(true);
    } else {
        setUpdatesEnabled(false);
        setPlainText(this->text);
        setUpdatesEnabled(true);
        isResetText = false;
    }
}

void CodeEditor::setHighlighter(QSyntaxHighlighter *high)
{
    if (highlighter != high) {
        highlighter = high;
    }
    if (highlighter && highlighter->document() != document())
        highlighter->setDocument(document());
    if (highlighter)
        highlighter->setSyntaxStyle(QSyntaxStyle::defaultStyle());
}
