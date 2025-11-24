#ifndef MARKDOWN_INLINE_ELEMENT_H
#define MARKDOWN_INLINE_ELEMENT_H

#pragma once
#include <string>

enum class InlineType {
    Bold,
    Italic,
    Code,
    Null
};

class Markdown_InlineElement {
private:
    InlineType type;
    size_t begin;
    size_t end;
public:
    Markdown_InlineElement();
    Markdown_InlineElement(InlineType T, size_t b, size_t e);
    InlineType getType() const;
    size_t getBegin() const;
    size_t getEnd() const;
};

#endif // MARKDOWN_INLINE_ELEMENT_H
