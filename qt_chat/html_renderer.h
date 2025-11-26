#ifndef HTML_RENDERER_H
#define HTML_RENDERER_H

#pragma once

#include <string>
#include "markdown_block_element.h"
#include "markdown_inline_element.h"

class Html_Renderer {
private:
    std::string HtmlText = "";
    std::string Style_css = "";
public:
    void SetStyle(std::string filename);
    void Init();
    void Tail();
    // 为块元素语法部分渲染 html 标签
    void BlockHtml(Markdown_BlockElement BlockElem);
    // 为行内元素语法部分渲染 html 标签
    std::string InlineHtml(LineElement Line);
    std::string getHtml() const;
};

#endif // HTML_RENDERER_H
