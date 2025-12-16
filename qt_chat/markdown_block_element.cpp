#include "markdown_block_element.h"

MarkdownBlockElement::MarkdownBlockElement(BlockType type, std::vector<LineElement> text)
    : type(type), text(text)
{
}

BlockType MarkdownBlockElement::getType() const
{
    return type;
}

std::vector<LineElement> MarkdownBlockElement::getText() const
{
    return text;
}
