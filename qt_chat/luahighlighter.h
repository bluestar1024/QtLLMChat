#ifndef LUAHIGHLIGHTER_H
#define LUAHIGHLIGHTER_H

#pragma once
#include "stylesyntaxhighlighter.h"
#include "language.h"

#include <QtCore/QRegularExpression>
#include <QtCore/QXmlStreamReader>
#include <QtGui/QTextDocument>
#include <QtCore/QVector>

class LuaHighlighter : public StyleSyntaxHighlighter
{
    Q_OBJECT
public:
    explicit LuaHighlighter(QObject *parent = nullptr);
    ~LuaHighlighter();

protected:
    virtual void highlightBlock(const QString &text) override = 0;

private:
    void loadLanguageFile(const QString &fileName);
    void initRules();

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

    QVector<HighlightRule> highlightRules;
    QVector<HighlightBlockRule> highlightBlockRules;
    QRegularExpression requirePattern;
    QRegularExpression functionPattern;
    QRegularExpression defTypePattern;
};

#endif // LUAHIGHLIGHTER_H
