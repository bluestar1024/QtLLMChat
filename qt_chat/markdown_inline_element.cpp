#include "markdown_inline_element.h"

MarkdownInlineElement::MarkdownInlineElement() : type(InlineType::Null), begin(0), end(0) { }
MarkdownInlineElement::MarkdownInlineElement(InlineType t, size_t b, size_t e)
    : type(t), begin(b), end(e)
{
}
MarkdownInlineElement::MarkdownInlineElement(InlineType t, size_t b, size_t e, std::string u)
    : type(t), begin(b), end(e), url(u)
{
}
InlineType MarkdownInlineElement::getType() const
{
    return type;
}
size_t MarkdownInlineElement::getBegin() const
{
    return begin;
}
size_t MarkdownInlineElement::getEnd() const
{
    return end;
}
std::string MarkdownInlineElement::getUrl() const
{
    return url;
}
