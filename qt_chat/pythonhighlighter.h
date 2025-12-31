#ifndef PYTHONHIGHLIGHTER_H
#define PYTHONHIGHLIGHTER_H

#pragma once
#include "stylesyntaxhighlighter.h"
#include "language.h"

#include <QtGui/QTextDocument>
#include <QtCore/QRegularExpression>
#include <QtCore/QXmlStreamReader>

class PythonHighlighter : public StyleSyntaxHighlighter
{
    Q_OBJECT
public:
    explicit PythonHighlighter(QObject *parent = nullptr);
    ~PythonHighlighter();

protected:
    virtual void highlightBlock(const QString &text) override;

private:
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

    void loadLanguageFile(const QString &fileName);
    void initRules();
    void singleLineStrHighlight(const QString &text, int start, int end);

    QVector<HighlightRule> highlightRules;
    QVector<HighlightRule> highlightStringRules;
    QVector<HighlightBlockRule> highlightBlockRules;
    QRegularExpression includePattern;
};

#endif // PYTHONHIGHLIGHTER_H
