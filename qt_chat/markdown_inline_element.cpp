#include "markdown_inline_element.h"

Markdown_InlineElement::Markdown_InlineElement()
    : type(InlineType::Null), begin(0), end(0) {
}
Markdown_InlineElement::Markdown_InlineElement(InlineType t, size_t b, size_t e)
    : type(t), begin(b), end(e) {}
Markdown_InlineElement::Markdown_InlineElement(InlineType t, size_t b, size_t e, std::string u)
    : type(t), begin(b), end(e), url(u) {}
InlineType Markdown_InlineElement::getType() const {
    return type;
}
size_t Markdown_InlineElement::getBegin() const {
    return begin;
}
size_t Markdown_InlineElement::getEnd() const {
    return end;
}
std::string Markdown_InlineElement::getUrl() const {
    return url;
}
