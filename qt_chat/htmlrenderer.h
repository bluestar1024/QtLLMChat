#ifndef HTMLRENDERER_H
#define HTMLRENDERER_H

#pragma once
#include "markdownblockelement.h"
#include "markdowninlineelement.h"

#include <QtCore/QString>

class HtmlRenderer
{
private:
    QString htmlText = "";
    QString styleCss = "";

public:
    void setStyle(QString filename);
    void init();
    void tail();
    // 为块元素语法部分渲染 html 标签
    void blockHtml(MarkdownBlockElement blockElem);
    // 为行内元素语法部分渲染 html 标签
    QString inlineHtml(LineElement line);
    QString getHtml() const;
};

#endif // HTMLRENDERER_H
