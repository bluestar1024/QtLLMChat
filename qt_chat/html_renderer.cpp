#include "html_renderer.h"

#include <math.h>

void HtmlRenderer::setStyle(std::string filename)
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
    case BlockType::Paragraph: {
        for (size_t i = 0; i < blockElem.getText().size(); i++) {
            if (!blockElem.getText()[i].text.empty()) {
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
            if (!blockElem.getText()[i].text.empty()) {
                htmlText += "\t<p>" + inlineHtml(blockElem.getText()[i]) + "</p>\n";
            }
        }
        htmlText += "</blockquote>\n";
        break;
    }
    case BlockType::UnorderedList: {
        htmlText += "<ul>\n";
        for (size_t i = 0; i < blockElem.getText().size(); i++) {
            if (!blockElem.getText()[i].text.empty()) {
                htmlText += "\t<li>" + inlineHtml(blockElem.getText()[i]) + "</li>\n";
            }
        }
        htmlText += "</ul>\n";
        break;
    }
    case BlockType::OrderedList: {
        htmlText += "<ol>\n";
        for (size_t i = 0; i < blockElem.getText().size(); i++) {
            if (!blockElem.getText()[i].text.empty()) {
                htmlText += "\t<li>" + inlineHtml(blockElem.getText()[i]) + "</li>\n";
            }
        }
        htmlText += "</ol>\n";
        break;
    }
    }
}

std::string HtmlRenderer::inlineHtml(LineElement line)
{
    std::vector<size_t> ins;
    std::string res = "";
    bool isContinue = false;
    size_t jBegin = 0;
    for (size_t i = 0; i < line.inlineElement.size(); i++) {
        ins.push_back(line.inlineElement[i].getBegin());
        ins.push_back(line.inlineElement[i].getEnd());
    }
    for (size_t i = 0; i < line.text.size(); i++) {
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
                    std::string alt = line.text.substr(ins[j], ins[j + 1] - ins[j]);
                    std::string url = line.inlineElement[j / 2].getUrl();
                    res += "<img src=\"" + url + "\" alt=\"" + alt + "\" />";
                    isContinue = true;
                    jBegin = j;
                    break;
                }
                case InlineType::Link: {
                    std::string text = line.text.substr(ins[j], ins[j + 1] - ins[j]);
                    std::string url = line.inlineElement[j / 2].getUrl();
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

std::string HtmlRenderer::getHtml() const
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
