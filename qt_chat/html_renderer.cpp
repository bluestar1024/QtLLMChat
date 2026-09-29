#include "html_renderer.h"

#include <math.h>

void HtmlRenderer::setStyle(QString filename)
{
    styleCss = filename;
}

void HtmlRenderer::blockHtml(MarkdownBlockElement blockElem)
{
    switch (blockElem.getType()) {
    case BlockType::Headinglevel1: {
        htmlText += "<h1>" + inlineHtml(blockElem.getText()[0]) + "</h1>\n";
        break;
    }
    case BlockType::Headinglevel2: {
        htmlText += "<h2>" + inlineHtml(blockElem.getText()[0]) + "</h2>\n";
        break;
    }
    case BlockType::Headinglevel3: {
        htmlText += "<h3>" + inlineHtml(blockElem.getText()[0]) + "</h3>\n";
        break;
    }
    case BlockType::Headinglevel4: {
        htmlText += "<h4>" + inlineHtml(blockElem.getText()[0]) + "</h4>\n";
        break;
    }
    case BlockType::Headinglevel5: {
        htmlText += "<h5>" + inlineHtml(blockElem.getText()[0]) + "</h5>\n";
        break;
    }
    case BlockType::Headinglevel6: {
        htmlText += "<h6>" + inlineHtml(blockElem.getText()[0]) + "</h6>\n";
        break;
    }
    case BlockType::Paragraph: {
        for (size_t i = 0; i < blockElem.getText().size(); i++) {
            if (!blockElem.getText()[i].text.isEmpty()) {
                htmlText += "<p>" + inlineHtml(blockElem.getText()[i]) + "</p>\n";
            }
        }
        break;
    }
    case BlockType::HorizontalRules: {
        htmlText += "<hr>\n";
        break;
    }
    case BlockType::CodeBlocks: {
        htmlText += "<pre><code class=\"language-" + blockElem.getText()[0].text + "\">";
        for (size_t i = 1; i < blockElem.getText().size(); i++) {
            htmlText += blockElem.getText()[i].text + "\n";
        }
        htmlText += "</code></pre>\n";
        break;
    }
    case BlockType::BlockQuote: {
        htmlText += "<blockquote>\n";
        for (size_t i = 0; i < blockElem.getText().size(); i++) {
            if (!blockElem.getText()[i].text.isEmpty()) {
                htmlText += "\t<p>" + inlineHtml(blockElem.getText()[i]) + "</p>\n";
            }
        }
        htmlText += "</blockquote>\n";
        break;
    }
    case BlockType::UnorderedList:
    case BlockType::OrderedList: {
        // 栈式嵌套渲染：LineElement::level 由 blockParse 按缩进推导，
        // 加深即开启子列表（嵌在父项 <li> 内，故 <li> 延迟到下一同级项
        // 或层级回退时才闭合）；每层标签取该层首行的标记类型
        // （有序 "1." 渲染 <ol>，无序 "- " 渲染 <ul>）
        struct ListLevel
        {
            int level;
            QString tag;
            bool liOpen;
        };
        std::vector<ListLevel> stack;
        std::vector<LineElement> lines = blockElem.getText();
        for (size_t i = 0; i < lines.size(); i++) {
            const LineElement &line = lines[i];
            if (line.text.isEmpty())
                continue;
            const QString tag = line.ordered ? "ol" : "ul";
            // 回退：闭合更深的层级（先补该层未闭合的 </li>）
            while (!stack.empty() && stack.back().level > line.level) {
                if (stack.back().liOpen)
                    htmlText += "</li>\n";
                htmlText += "</" + stack.back().tag + ">\n";
                stack.pop_back();
            }
            // 加深：开启新的子列表层级（此时上层 <li> 保持未闭合以嵌套）
            while (stack.empty() || stack.back().level < line.level) {
                QString openTag = "<" + tag;
                if (tag == "ul") {
                    // 无序列表符号按该列表自身的嵌套深度显式指定（与 ol 无关）：
                    // 第一级 disc（实心圆）、第二级 circle（空心圆）、第三级起
                    // square（实心方块）。浏览器默认按 ol/ul 混合深度取符号
                    //（ol 内首层 ul 也显示 circle），与预期不符
                    int ulDepth = 1;
                    for (size_t k = 0; k < stack.size(); k++) {
                        if (stack[k].tag == "ul")
                            ++ulDepth;
                    }
                    const QString marker =
                            ulDepth >= 3 ? "square" : (ulDepth == 2 ? "circle" : "disc");
                    openTag += " style=\"list-style-type:" + marker + "\"";
                }
                htmlText += openTag + ">\n";
                ListLevel lv;
                lv.level = line.level;
                lv.tag = tag;
                lv.liOpen = false;
                stack.push_back(lv);
            }
            // 同级列表项：闭合上一个 <li>
            if (stack.back().liOpen)
                htmlText += "</li>\n";
            htmlText += "<li>" + inlineHtml(line);
            stack.back().liOpen = true;
        }
        while (!stack.empty()) {
            if (stack.back().liOpen)
                htmlText += "</li>\n";
            htmlText += "</" + stack.back().tag + ">\n";
            stack.pop_back();
        }
        break;
    }
    }
}

QString HtmlRenderer::inlineHtml(LineElement line)
{
    std::vector<size_t> ins;
    QString res = "";
    bool isContinue = false;
    size_t jBegin = 0;
    for (size_t i = 0; i < line.inlineElement.size(); i++) {
        ins.push_back(line.inlineElement[i].getBegin());
        ins.push_back(line.inlineElement[i].getEnd());
    }
    // 循环上界取到 size()：当行内标记恰好结束于行尾时（如标题 "### 1. **xxx**"
    // 解析后粗体 end == 文本长度），结束标签应插入的位置等于文本长度，
    // 若上界仍为 size() 之前，则 </strong> 等结束标签永远不会被写入
    for (size_t i = 0; i <= size_t(line.text.size()); i++) {
        for (size_t j = 0; j < ins.size(); j++) {
            if (ins[j] == i && (j % 2 == 0)) {
                switch (line.inlineElement[j / 2].getType()) {
                case InlineType::Bold: {
                    res += "<strong>";
                    break;
                }
                case InlineType::Italic: {
                    res += "<em>";
                    break;
                }
                case InlineType::Code: {
                    res += "<code>";
                    break;
                }
                case InlineType::Image: {
                    QString alt = line.text.mid(ins[j], ins[j + 1] - ins[j]);
                    QString url = line.inlineElement[j / 2].getUrl();
                    res += "<img src=\"" + url + "\" alt=\"" + alt + "\" />";
                    isContinue = true;
                    jBegin = j;
                    break;
                }
                case InlineType::Link: {
                    QString text = line.text.mid(ins[j], ins[j + 1] - ins[j]);
                    QString url = line.inlineElement[j / 2].getUrl();
                    res += "<a href=\"" + url + "\">" + text + "</a>";
                    isContinue = true;
                    jBegin = j;
                    break;
                }
                default:
                    break;
                }
            } else if (ins[j] == i && (j % 2 == 1)) {
                switch (line.inlineElement[(j - 1) / 2].getType()) {
                case InlineType::Bold: {
                    res += "</strong>";
                    break;
                }
                case InlineType::Italic: {
                    res += "</em>";
                    break;
                }
                case InlineType::Code: {
                    res += "</code>";
                }
                case InlineType::Image:
                    break;
                case InlineType::Link:
                    break;
                default:
                    break;
                }
            }
        }
        // i == size() 仅用于补写位于行尾的结束标签，没有对应字符可输出
        if (i == line.text.size())
            break;
        if (isContinue) {
            if (i >= ins[jBegin + 1]) {
                isContinue = false;
                res += line.text[i];
            }
        } else
            res += line.text[i];
    }
    return res;
}

QString HtmlRenderer::getHtml() const
{
    return htmlText;
}

void HtmlRenderer::init()
{
    htmlText += "<!DOCTYPE html>\n";
    htmlText += "<html lang=\"en\">\n";
    htmlText += "<head>\n";
    htmlText += "  <meta charset=\"UTF-8\" />\n";
    htmlText += "  <title> CMark++ </title>\n";
    htmlText += "  <link rel=\"stylesheet\" href=\"";
    htmlText += styleCss;
    htmlText += "\" />\n";
    htmlText += "  <link rel=\"stylesheet\" href=\"github-dark.min.css\" />\n";
    htmlText += "</head>\n\n";
    htmlText += "<body>\n";
}

void HtmlRenderer::tail()
{
    htmlText += "<script src=\"highlight.min.js\"></script>\n";
    htmlText += "<script>hljs.highlightAll();</script>\n";
    htmlText += "</body>\n";
    htmlText += "</html>\n";
}
