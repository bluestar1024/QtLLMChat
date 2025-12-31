#ifndef GLSLHIGHLIGHTER_H
#define GLSLHIGHLIGHTER_H

#pragma once
#include "stylesyntaxhighlighter.h"
#include "language.h"

#include <QtCore/QRegularExpression>
#include <QtGui/QTextCharFormat>
#include <QtCore/QVector>

class GLSLHighlighter : public StyleSyntaxHighlighter
{
    Q_OBJECT
public:
    explicit GLSLHighlighter(QObject *parent = nullptr);
    ~GLSLHighlighter();

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
    QRegularExpression functionPattern;
    QRegularExpression commentStartPattern;
    QRegularExpression commentEndPattern;
};

#endif // GLSLHIGHLIGHTER_H
