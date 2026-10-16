#ifndef MARKDOWNPARSER_H
#define MARKDOWNPARSER_H

#pragma once
#include "markdownblockelement.h"
#include "markdowninlineelement.h"

#include <vector>
// #include <unordered_map>

#include <QtCore/QString>
#include <QtCore/QHash>

class MarkdownParser
{
public:
    std::vector<std::vector<QString>> rawBlock;
    // std::unordered_map<QString, QString> refLinks;
    QHash<QString, QString> refLinks;
    void split(const QString &rawText);
    std::vector<MarkdownInlineElement> inlineParse(const QString &rawText,
                                                   QString &resText);
    void blockParse(const QString &rawText, std::vector<MarkdownBlockElement> &blockElem);
    bool isHorizontalRules(const QString &lineStr, const QString *prevLine = nullptr);
};

#endif // MARKDOWNPARSER_H
