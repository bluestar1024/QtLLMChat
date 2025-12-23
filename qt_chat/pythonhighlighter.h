#ifndef PYTHONHIGHLIGHTER_H
#define PYTHONHIGHLIGHTER_H

#pragma once
#include "stylesyntaxhighlighter.h"
#include "languang.h"

#include <QtGui/QTextDocument>
#include <QtCore/QRegularExpression>
#include <QtCore/QXmlStreamReader>

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
    ~PythonHighlighter();

protected:
    virtual void highlightBlock(const QString &text) override;

private:
    void loadLanguageFile(const QString &fileName);
    void initRules();
    void singleLineStrHighlight(const QString &text, int start, int end);

    QVector<HighlightRule> highlightRules;
    QVector<HighlightRule> highlightStringRules;
    QVector<HighlightBlockRule> highlightBlockRules;
    QRegularExpression includePattern;
};

#endif // PYTHONHIGHLIGHTER_H
