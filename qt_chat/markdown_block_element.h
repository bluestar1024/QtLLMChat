#ifndef MARKDOWN_BLOCK_ELEMENT_H
#define MARKDOWN_BLOCK_ELEMENT_H

#pragma once
#include "markdown_inline_element.h"

#include <vector>

#include <QtCore/QString>

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

struct LineElement
{
    QString text;
    std::vector<MarkdownInlineElement> inlineElement;
    LineElement(QString t) : text(t) { }
    LineElement(QString t, std::vector<MarkdownInlineElement> i) : text(t), inlineElement(i) { }
};

class MarkdownBlockElement
{
private:
    BlockType type;
    std::vector<LineElement> text;

public:
    MarkdownBlockElement(BlockType type, std::vector<LineElement> text);
    BlockType getType() const;
    std::vector<LineElement> getText() const;
};

#endif // MARKDOWN_BLOCK_ELEMENT_H
