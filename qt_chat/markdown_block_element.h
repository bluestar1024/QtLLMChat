#ifndef MARKDOWN_BLOCK_ELEMENT_H
#define MARKDOWN_BLOCK_ELEMENT_H

#pragma once
#include <string>
#include <vector>
#include "markdown_inline_element.h"

// 枚举语法类型
enum class BlockType {
    Paragraph,
    Headinglevel1,
    Headinglevel2,
    Headinglevel3,
    HorizontalRules,
    CodeBlocks,
    BlockQuote,
    OrderedList,
    UnorderedList
};

struct LineElement {
    std::string text;
    std::vector<Markdown_InlineElement> InlineElement;
    LineElement(std::string t) : text(t) {}
    LineElement(std::string t, std::vector<Markdown_InlineElement> I) : text(t), InlineElement(I) {}
};

class Markdown_BlockElement {
private:
    BlockType type;
    std::vector<LineElement> Text;
public:
    Markdown_BlockElement(BlockType t, std::vector<LineElement>T);
    BlockType getType() const;
    std::vector<LineElement> getText() const;
};

#endif // MARKDOWN_BLOCK_ELEMENT_H
