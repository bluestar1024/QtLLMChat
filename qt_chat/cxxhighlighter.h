#ifndef CXXHIGHLIGHTER_H
#define CXXHIGHLIGHTER_H

#pragma once
#include "stylesyntaxhighlighter.h"
#include "language.h"

#include <QtGui/QTextDocument>
#include <QtCore/QRegularExpression>
#include <QtCore/QVector>
#include <QtCore/QXmlStreamReader>
#include <QtGui/QTextCharFormat>

class CXXHighlighter : public StyleSyntaxHighlighter
{
    Q_OBJECT
public:
    explicit CXXHighlighter(QObject *parent = nullptr);
    ~CXXHighlighter();

protected:
    virtual void highlightBlock(const QString &text) override;

private:
    void loadLanguageFile(const QString &fileName);
    void initRules();

    struct HighlightRule
    {
        QRegularExpression pattern;
        QString format;
    };

    QVector<HighlightRule> highlightRules;
    QRegularExpression includePattern;
    QRegularExpression commentStartPattern;
    QRegularExpression commentEndPattern;
};

#endif // CXXHIGHLIGHTER_H
