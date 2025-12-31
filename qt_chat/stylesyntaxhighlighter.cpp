#include "stylesyntaxhighlighter.h"

StyleSyntaxHighlighter::StyleSyntaxHighlighter(QObject *parent)
    : QSyntaxHighlighter(parent), syntaxStyle(nullptr)
{
}

StyleSyntaxHighlighter::~StyleSyntaxHighlighter() { }

void StyleSyntaxHighlighter::setSyntaxStyle(SyntaxStyle *style)
{
    syntaxStyle = style;
}

SyntaxStyle *StyleSyntaxHighlighter::getSyntaxStyle() const
{
    return syntaxStyle;
}
