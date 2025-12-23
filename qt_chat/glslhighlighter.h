#ifndef GLSLHIGHLIGHTER_H
#define GLSLHIGHLIGHTER_H

#pragma once
#include "stylesyntaxhighlighter.h"
#include "languang.h"

#include <QRegularExpression>
#include <QTextCharFormat>
#include <QVector>
#include <QPair>

class GLSLHighlighter : public StyleSyntaxHighlighter
{
    Q_OBJECT
public:
    explicit GLSLHighlighter(QTextDocument *parent = nullptr);

protected:
    virtual void highlightBlock(const QString &text) override;

private:
    void loadLanguageFile(const QString &fileName);
    void initRules();

    using HighlightRule = QPair<QRegularExpression, QString>;
    QVector<HighlightRule> highlightRules;

    QRegularExpression includePattern;
    QRegularExpression functionPattern;
    QRegularExpression defTypePattern;
    QRegularExpression commentStartPattern;
    QRegularExpression commentEndPattern;
};

#endif // GLSLHIGHLIGHTER_H
