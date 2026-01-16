#include "codeeditor.h"
#include "globalvariables.h"

#include <QtGui/QFontMetrics>
#include <QtGui/QTextBlock>
#include <QtGui/QTextDocument>
#include <QtGui/QPalette>
#include <QtGui/QAbstractTextDocumentLayout>

CodeEditor::CodeEditor(int maxWidth, QWidget *parent)
    : QTextEdit(parent), text(""), lexerName(""), isResetText(false)
{
    // qDebug() << "CodeEditor start" << this;
    setFixedWidth(maxWidth);
    horizontalScrollBar()->setCursor(Qt::PointingHandCursor);

    // qDebug() << "CodeEditor ing0" << this;
    QFont font = QFontDatabase::systemFont(QFontDatabase::FixedFont);
    font.setFixedPitch(true);
    font.setPointSize(windowFontPointSize);
    setFont(font);

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
        codeThemeFilePath = ":/config/light_theme.xml";
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
        codeThemeFilePath = ":/config/dark_theme.xml";
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

void CodeEditor::adjustHeight()
{
    // qDebug() << "CodeEditor adjustHeight start" << this;
    int h = int(document()->size().height()) + 15;
    setFixedHeight(h);
    updateLineNumberAreaWidth();
    lineNumberArea->update();
    emit setSizeFinished();
    // qDebug() << "CodeEditor adjustHeight end" << this;
}

void CodeEditor::resizeEvent(QResizeEvent *e)
{
    // qDebug() << "CodeEditor resizeEvent start" << this;
    QTextEdit::resizeEvent(e);
    if (height() != int(document()->size().height()) + 15)
        adjustHeight();
    // qDebug() << "CodeEditor resizeEvent end" << this;
}

void CodeEditor::highlightCode(const QString &text, const QString &lexer)
{
    // qDebug() << "CodeEditor highlightCode start" << this;
    QString appendText = text;
    if (!this->text.isEmpty() && text.contains(this->text))
        appendText = text.mid(this->text.length());
    if (appendText == text && this->text.contains(text))
        isResetText = true;
    this->text = text;
    lexerName = lexer;

    // qDebug() << "CodeEditor highlightCode ing0" << this;
    StyleSyntaxHighlighter *next = nullptr;
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

    // qDebug() << "CodeEditor highlightCode ing1" << this;
    // qDebug() << "text:" << text << this;
    // qDebug() << "lexer:" << lexer << this;
    // qDebug() << "isResetText:" << isResetText << this;
    if (!isResetText) {
        // qDebug() << "CodeEditor highlightCode ing2" << this;
        setUpdatesEnabled(false);
        // qDebug() << "CodeEditor highlightCode ing6" << this;
        QTextCursor c(document());
        // qDebug() << "CodeEditor highlightCode ing7" << this;
        c.movePosition(QTextCursor::End);
        c.beginEditBlock();
        c.insertText(appendText);
        // qDebug() << "CodeEditor highlightCode ing8" << this;
        c.endEditBlock();
        // qDebug() << "CodeEditor highlightCode ing9" << this;
        setUpdatesEnabled(true);
        // qDebug() << "CodeEditor highlightCode ing3" << this;
    } else {
        // qDebug() << "CodeEditor highlightCode ing4" << this;
        setUpdatesEnabled(false);
        setPlainText(this->text);
        setUpdatesEnabled(true);
        isResetText = false;
        // qDebug() << "CodeEditor highlightCode ing5" << this;
    }
    // qDebug() << "CodeEditor highlightCode end" << this;
}

void CodeEditor::setHighlighter(StyleSyntaxHighlighter *high)
{
    if (highlighter != high) {
        highlighter = high;
    }
    if (highlighter && highlighter->document() != document())
        highlighter->setDocument(document());
    if (highlighter)
        highlighter->setSyntaxStyle(SyntaxStyle::defaultStyle());
}
