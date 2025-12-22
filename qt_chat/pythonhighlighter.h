#ifndef PYTHONHIGHLIGHTER_H
#define PYTHONHIGHLIGHTER_H

#pragma once
#include "syntaxstyle.h"
#include "stylesyntaxhighlighter.h"

#include <QTextDocument>
#include <QRegularExpression>
#include <QXmlStreamReader>
#include <QFile>

struct HighlightRule
{
    QRegularExpression pattern;
    QString formatName;
};

struct HighlightBlockRule
{
    QRegularExpression begin;
    QRegularExpression end;
    QString formatName;
};

class PythonHighlighter : public StyleSyntaxHighlighter
{
    Q_OBJECT
public:
    explicit PythonHighlighter(QTextDocument *doc = nullptr);
    void setSyntaxStyle(SyntaxStyle *style);

protected:
    virtual void highlightBlock(const QString &text) override;

private:
    void loadLanguageFile(const QString &fileName);
    void initRules();
    void singleLineStrHighlight(const QString &text, int start, int end);

    SyntaxStyle *style;
    QVector<HighlightRule> highlightRules;
    QVector<HighlightRule> highlightStringRules;
    QVector<HighlightBlockRule> highlightBlockRules;
    QRegularExpression includePattern;
};

#endif // PYTHONHIGHLIGHTER_H
