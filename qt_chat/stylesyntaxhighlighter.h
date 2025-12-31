#ifndef STYLESYNTAXHIGHLIGHTER_H
#define STYLESYNTAXHIGHLIGHTER_H

#pragma once
#include "syntaxstyle.h"

#include <QtGui/QSyntaxHighlighter>
#include <QtGui/QTextDocument>

class StyleSyntaxHighlighter : public QSyntaxHighlighter
{
    Q_OBJECT
public:
    explicit StyleSyntaxHighlighter(QObject *parent = nullptr);
    ~StyleSyntaxHighlighter();

    void setSyntaxStyle(SyntaxStyle *style);
    SyntaxStyle *getSyntaxStyle() const;

private:
    SyntaxStyle *syntaxStyle;
};

#endif // STYLESYNTAXHIGHLIGHTER_H
