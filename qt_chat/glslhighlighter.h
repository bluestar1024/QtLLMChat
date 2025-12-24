#ifndef GLSLHIGHLIGHTER_H
#define GLSLHIGHLIGHTER_H

#pragma once
#include "stylesyntaxhighlighter.h"
#include "languang.h"

#include <QtCore/QRegularExpression>
#include <QtGui/QTextCharFormat>
#include <QtCore/QVector>

struct HighlightRule
{
    QRegularExpression pattern;
    QString format;
};

class GLSLHighlighter : public StyleSyntaxHighlighter
{
    Q_OBJECT
public:
    explicit GLSLHighlighter(QTextDocument *parent = nullptr);
    ~GLSLHighlighter();

protected:
    virtual void highlightBlock(const QString &text) override;

private:
    void loadLanguageFile(const QString &fileName);
    void initRules();

    QVector<HighlightRule> highlightRules;
    QRegularExpression includePattern;
    QRegularExpression functionPattern;
    QRegularExpression commentStartPattern;
    QRegularExpression commentEndPattern;
};

#endif // GLSLHIGHLIGHTER_H
