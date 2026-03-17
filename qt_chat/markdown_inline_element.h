#ifndef MARKDOWN_INLINE_ELEMENT_H
#define MARKDOWN_INLINE_ELEMENT_H

#pragma once
#include <QtCore/QString>

enum class InlineType { Bold, Italic, Code, Image, Link, Null };

class MarkdownInlineElement
{
private:
    InlineType type;
    size_t begin;
    size_t end;
    QString url;

public:
    MarkdownInlineElement();
    MarkdownInlineElement(InlineType t, size_t b, size_t e);
    MarkdownInlineElement(InlineType t, size_t b, size_t e, QString u);
    InlineType getType() const;
    size_t getBegin() const;
    size_t getEnd() const;
    QString getUrl() const;
};

#endif // MARKDOWN_INLINE_ELEMENT_H
