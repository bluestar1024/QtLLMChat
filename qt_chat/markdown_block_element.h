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
    Headinglevel4,
    Headinglevel5,
    Headinglevel6,
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
    // 列表嵌套级别（0 为顶级，仅列表块使用）与该行标记是否有序（决定嵌套子层标签）
    int level = 0;
    bool ordered = false;
    LineElement(QString t) : text(t) { }
    LineElement(QString t, std::vector<MarkdownInlineElement> i) : text(t), inlineElement(i) { }
    LineElement(QString t, std::vector<MarkdownInlineElement> i, int lv, bool od)
        : text(t), inlineElement(i), level(lv), ordered(od)
    { }
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
