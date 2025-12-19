#ifndef CODEEDITOR_H
#define CODEEDITOR_H

#pragma once
#include "linenumberarea.h"
#include "syntaxstyle.h"
#include "syntaxhighlighter.h"

#include <QtWidgets/QTextEdit>
#include <QtWidgets/QScrollBar>
#include <QtGui/QPainter>
#include <QtGui/QTextCursor>
#include <QtGui/QFontDatabase>

extern const int windowFontPointSize;
extern const QString codeThemeFilePath;

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
    void setHighlighter(QSyntaxHighlighter *high);

    LineNumberArea *lineNumberArea;
    QHash<QString, QSyntaxHighlighter *> highlighters;
    QSyntaxHighlighter *highlighter;
    QString text;
    QString lexerName;
    bool isResetText;
};

#endif // CODEEDITOR_H
