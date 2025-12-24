#ifndef LUAHIGHLIGHTER_H
#define LUAHIGHLIGHTER_H

#pragma once
#include "stylesyntaxhighlighter.h"

#include <QtCore/QRegularExpression>
#include <QtCore/QXmlStreamReader>
#include <QtGui/QTextDocument>
#include <QtCore/QVector>

struct HighlightRule
{
    QRegularExpression pattern;
    QString format;
};

struct HighlightBlockRule
{
    QRegularExpression start;
    QRegularExpression end;
    QString format;
};

class LuaHighlighter : public StyleSyntaxHighlighter
{
    Q_OBJECT
public:
    explicit LuaHighlighter(QTextDocument *parent = nullptr);
    ~LuaHighlighter();

protected:
    virtual void highlightBlock(const QString &text) override;

private:
    void loadLanguageFile(const QString &fileName);
    void initRules();

    QVector<HighlightRule> highlightRules;
    QVector<HighlightBlockRule> highlightBlockRules;
    QRegularExpression requirePattern;
    QRegularExpression functionPattern;
    QRegularExpression defTypePattern;
};

#endif // LUAHIGHLIGHTER_H
