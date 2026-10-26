#ifndef MARKDOWNBLOCKELEMENT_H
#define MARKDOWNBLOCKELEMENT_H

#pragma once
#include "markdowninlineelement.h"

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
    // 有序列表项的原文序号（如 "10." 即 10），非有序项或未取到为 0。
    // 有序列表被代码围栏、标题等中断后，split 会另起一个列表块；渲染器据
    // 此序号输出 <ol start>/<li value>，避免新块从 1 重新编号
    int number = 0;
    LineElement(QString t) : text(t) { }
    LineElement(QString t, std::vector<MarkdownInlineElement> i) : text(t), inlineElement(i) { }
    LineElement(QString t, std::vector<MarkdownInlineElement> i, int lv, bool od, int num = 0)
        : text(t), inlineElement(i), level(lv), ordered(od), number(num)
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

#endif // MARKDOWNBLOCKELEMENT_H
