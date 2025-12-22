#ifndef STYLESYNTAXHIGHLIGHTER_H
#define STYLESYNTAXHIGHLIGHTER_H

#pragma once
#include "syntaxstyle.h"

#include <QtGui/QSyntaxHighlighter>
#include <QtGui/QTextDocument>

class StyleSyntaxHighlighter : public SyntaxHighlighter
{
    Q_OBJECT
public:
    explicit StyleSyntaxHighlighter(QTextDocument *parent = nullptr);
    ~StyleSyntaxHighlighter();

    void setSyntaxStyle(SyntaxStyle *style);
    SyntaxStyle *getSyntaxStyle() const;

private:
    SyntaxStyle *syntaxStyle;
};

#endif // STYLESYNTAXHIGHLIGHTER_H
