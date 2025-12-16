#ifndef HTML_RENDERER_H
#define HTML_RENDERER_H

#pragma once
#include "markdown_block_element.h"
#include "markdown_inline_element.h"

#include <string>

class HtmlRenderer
{
private:
    std::string htmlText = "";
    std::string styleCss = "";

public:
    void setStyle(std::string filename);
    void init();
    void tail();
    // 为块元素语法部分渲染 html 标签
    void blockHtml(MarkdownBlockElement blockElem);
    // 为行内元素语法部分渲染 html 标签
    std::string inlineHtml(LineElement line);
    std::string getHtml() const;
};

#endif // HTML_RENDERER_H
