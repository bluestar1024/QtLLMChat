#ifndef MARKDOWN_PARSER_H
#define MARKDOWN_PARSER_H

#pragma once
#include "markdown_block_element.h"
#include "markdown_inline_element.h"

#include <string>
#include <vector>
#include <unordered_map>

class MarkdownParser
{
public:
    std::vector<std::vector<std::string>> rawBlock;
    std::unordered_map<std::string, std::string> refLinks;
    void split(const std::string &rawText);
    std::vector<MarkdownInlineElement> inlineParse(const std::string &rawText,
                                                   std::string &resText);
    void blockParse(const std::string &rawText, std::vector<MarkdownBlockElement> &blockElem);
    bool isHorizontalRules(const std::string &lineStr, const std::string *prevLine = nullptr);
};

#endif // MARKDOWN_PARSER_H
