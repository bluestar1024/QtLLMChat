#include "html_renderer.h"
#include <math.h>

void Html_Renderer::SetStyle(std::string filename) {
    Style_css = filename;
}

void Html_Renderer::BlockHtml(Markdown_BlockElement BlockElem) {
    switch (BlockElem.getType()) {
        case BlockType::Headinglevel1: {
            HtmlText += "<h1>" + InlineHtml(BlockElem.getText()[0]) + "</h1>\n";
            break;
        }
        case BlockType::Headinglevel2: {
            HtmlText += "<h2>" + InlineHtml(BlockElem.getText()[0]) + "</h2>\n";
            break;
        }
        case BlockType::Headinglevel3: {
            HtmlText += "<h3>" + InlineHtml(BlockElem.getText()[0]) + "</h3>\n";
            break;
        }
        case BlockType::Paragraph: {
            for(size_t i = 0; i < BlockElem.getText().size(); i++) {
                if (!BlockElem.getText()[i].text.empty()) {
                    HtmlText += "<p>" + InlineHtml(BlockElem.getText()[i]) + "</p>\n";
                }
            }
            break;
        }
        case BlockType::HorizontalRules: {
            HtmlText += "<hr>\n";
            break;
        }
        case BlockType::CodeBlocks: {
            HtmlText += "<pre><code class=\"language-" + BlockElem.getText()[0].text + "\">";
            for (size_t i = 1; i < BlockElem.getText().size(); i++) {
                HtmlText += BlockElem.getText()[i].text + "\n";
            }
            HtmlText += "</code></pre>\n";
            break;
        }
        case BlockType::BlockQuote: {
            HtmlText += "<blockquote>\n";
            for(size_t i = 0; i < BlockElem.getText().size(); i++) {
                if (!BlockElem.getText()[i].text.empty()) {
                    HtmlText += "\t<p>" + InlineHtml(BlockElem.getText()[i]) + "</p>\n";
                }
            }
            HtmlText += "</blockquote>\n";
            break;
        }
        case BlockType::UnorderedList: {
            HtmlText += "<ul>\n";
            for (size_t i = 0; i < BlockElem.getText().size(); i++) {
                if (!BlockElem.getText()[i].text.empty()) {
                    HtmlText += "\t<li>" + InlineHtml(BlockElem.getText()[i]) + "</li>\n";
                }
            }
            HtmlText += "</ul>\n";
            break;
        }
        case BlockType::OrderedList: {
            HtmlText += "<ol>\n";
            for (size_t i = 0; i < BlockElem.getText().size(); i++) {
                if (!BlockElem.getText()[i].text.empty()) {
                    HtmlText += "\t<li>" + InlineHtml(BlockElem.getText()[i]) + "</li>\n";
                }
            }
            HtmlText += "</ol>\n";
            break;
        }
    }
}

std::string Html_Renderer::InlineHtml(LineElement Line) {
    std::vector<size_t> ins;
    std::string res = "";
    bool isContinue = false;
    size_t jBegin = 0;
    for (size_t i = 0; i < Line.InlineElement.size(); i++) {
        ins.push_back(Line.InlineElement[i].getBegin());
        ins.push_back(Line.InlineElement[i].getEnd());
    }
    for (size_t i = 0; i < Line.text.size(); i++) {
        for (size_t j = 0; j < ins.size(); j++) {
            if (ins[j] == i && (j % 2 == 0)) {
                switch (Line.InlineElement[j / 2].getType()) {
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
                        std::string alt = Line.text.substr(ins[j], ins[j + 1] - ins[j]);
                        std::string url = Line.InlineElement[j / 2].getUrl();
                        res += "<img src=\"" + url + "\" alt=\"" + alt + "\" />";
                        isContinue = true;
                        jBegin = j;
                        break;
                    }
                    case InlineType::Link: {
                        std::string text = Line.text.substr(ins[j], ins[j + 1] - ins[j]);
                        std::string url = Line.InlineElement[j / 2].getUrl();
                        res += "<a href=\"" + url + "\">" + text + "</a>";
                        isContinue = true;
                        jBegin = j;
                        break;
                    }
                    default:
                        break;
                }
            }
            else if (ins[j] == i && (j % 2 == 1)) {
                switch (Line.InlineElement[(j - 1) / 2].getType()) {
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
                res += Line.text[i];
            }
        }
        else
            res += Line.text[i];
    }
    return res;
}

std::string Html_Renderer::getHtml() const {
    return HtmlText;
}

void Html_Renderer::Init() {
    HtmlText += "<!DOCTYPE html>\n";
    HtmlText += "<html lang=\"en\">\n";
    HtmlText += "<head>\n";
    HtmlText += "  <meta charset=\"UTF-8\" />\n";
    HtmlText += "  <title> CMark++ </title>\n";
    HtmlText += "  <link rel=\"stylesheet\" href=\"";
    HtmlText += Style_css;
    HtmlText += "\" />\n";
    HtmlText += "  <link rel=\"stylesheet\" href=\"github-dark.min.css\" />\n";
    HtmlText += "</head>\n\n";
    HtmlText += "<body>\n";
}

void Html_Renderer::Tail() {
    HtmlText += "<script src=\"highlight.min.js\"></script>\n";
    HtmlText += "<script>hljs.highlightAll();</script>\n";
    HtmlText += "</body>\n";
    HtmlText += "</html>\n";
}
