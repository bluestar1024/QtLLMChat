#ifndef CXXHIGHLIGHTER_H
#define CXXHIGHLIGHTER_H

#pragma once
#include "stylesyntaxhighlighter.h"

#include <QtGui/QTextDocument>
#include <QtCore/QRegularExpression>
#include <QtCore/QVector>
#include <QtCore/QXmlStreamReader>
#include <QtGui/QTextCharFormat>

struct HighlightRule
{
    QRegularExpression pattern;
    QString format;
};

class QCXXHighlighter : public StyleSyntaxHighlighter
{
    Q_OBJECT
public:
    explicit QCXXHighlighter(QTextDocument *parent = nullptr);
    ~QCXXHighlighter();

protected:
    virtual void highlightBlock(const QString &text) override;

private:
    void loadLanguageFile(const QString &fileName);
    void initRules();

    QVector<HighlightRule> highlightRules;
    QRegularExpression includePattern;
    QRegularExpression commentStartPattern;
    QRegularExpression commentEndPattern;
};

#endif // CXXHIGHLIGHTER_H
