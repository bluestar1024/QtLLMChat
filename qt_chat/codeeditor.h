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

class CodeEditor : public QTextEdit
{
    Q_OBJECT
public:
    explicit CodeEditor(int maxWidth = 810, QWidget *parent = nullptr);
    ~CodeEditor();

    void setThemeStyle(bool light = false);
    int getFirstVisibleBlockNumber() const;
    void highlightCode(const QString &text, const QString &lexer = "cpp");

public slots:
    void adjustHeight();

signals:
    void setSizeFinished();

protected:
    void resizeEvent(QResizeEvent *e) override;

private:
    void updateLineNumberAreaWidth();
    void setHighlighter(StyleSyntaxHighlighter *high);

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
