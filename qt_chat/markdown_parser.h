#ifndef MARKDOWN_PARSER_H
#define MARKDOWN_PARSER_H

#pragma once
#include <string>
#include <vector>
#include "markdown_block_element.h"
#include "markdown_inline_element.h"

class Markdown_Parser {
public:
    std::vector<std::vector<std::string>> RawBlock;
    void split(const std::string& RawText);
    std::vector<Markdown_InlineElement> inline_parse(const std::string& RawText, std::string& ResText);
    void block_parse(const std::string& RawText, std::vector<Markdown_BlockElement>& BlockElem);
};

#endif // MARKDOWN_PARSER_H
