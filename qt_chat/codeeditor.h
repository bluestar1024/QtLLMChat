#ifndef CODEEDITOR_H
#define CODEEDITOR_H

#pragma once
#include "linenumberarea.h"
#include "syntaxstyle.h"
#include "stylesyntaxhighlighter.h"
#include "pythonhighlighter.h"
#include "cxxhighlighter.h"
#include "glslhighlighter.h"
#include "luahighlighter.h"

#include <QtWidgets/QTextEdit>
#include <QtWidgets/QScrollBar>
#include <QtGui/QPainter>
#include <QtGui/QTextCursor>
#include <QtGui/QFontDatabase>

// extern const int windowFontPointSize;
// extern QString codeThemeFilePath;

class AppContext;

class CodeEditor : public QTextEdit
{
    Q_OBJECT
public:
    explicit CodeEditor(AppContext *appContext, int maxWidth = 810, QWidget *parent = nullptr);
    ~CodeEditor();

    void setThemeStyle(bool light = false);
    int getFirstVisibleBlockNumber() const;
    void highlightCode(const QString &text, const QString &lexer = "cpp");
    // 跨 DPI 屏拖动未释放鼠标时的轻量刷新：像素字号不随屏 DPI 自动变化，
    // 这里按新像素字号重设字体（含等宽回退判定）、宽度按比例缩放并重新量算高度，
    // 避免未重建前代码块字体/尺寸仍停留在旧屏比例
    void updateSize(qreal curDpi, qreal lastDpi);

public slots:
    void adjustHeight();

signals:
    void setSizeFinished();

protected:
    void resizeEvent(QResizeEvent *e) override;

private:
    void updateLineNumberAreaWidth();
    void setHighlighter(StyleSyntaxHighlighter *high);
    // 按 appContext->windowFontPixelSize() 选择等宽字体并应用（构造与跨屏
    // 刷新共用，保证两条路径的字体选择逻辑一致）
    void applyDpiFont();

    AppContext *appContext;

    LineNumberArea *lineNumberArea;
    QHash<QString, StyleSyntaxHighlighter *> highlighters;
    StyleSyntaxHighlighter *highlighter;
    QString text;
    QString lexerName;
    bool isResetText;
    // 析构期间置位：子对象（高亮器）析构可能触发 textChanged 等回调
    // 重入 adjustHeight，向已销毁的外层控件传播 setSizeFinished 信号
    bool isDestroying;
};

#endif // CODEEDITOR_H
