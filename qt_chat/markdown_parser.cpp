#include "markdown_parser.h"

#include <sstream>
#include <regex>
#include <iostream>

void MarkdownParser::split(const std::string &rawText)
{
    rawBlock.clear();
    std::istringstream iss(rawText);
    std::vector<std::string> rawLine;
    std::string lineText;
    while (std::getline(iss, lineText)) {
        rawLine.push_back(lineText);
    }

    std::vector<std::string> blockText;
    size_t ins = 0;
    bool codeFlag = false;
    bool orderedListsFlag = false;
    bool unorderedListFlag = false;
    bool blockQuoteFlag = false;

    while (ins < rawLine.size()) {
        const std::string *prev = (ins > 0) ? &rawLine[ins - 1] : nullptr;
        const std::string &curr = rawLine[ins];
        const std::string *next = (ins + 1 < rawLine.size()) ? &rawLine[ins + 1] : nullptr;
        if (curr.empty()) {
            blockText.push_back(curr);
            ins++;
            continue;
        }
        if (curr.rfind("```", 0) == 0 || codeFlag) {
            if (curr.rfind("```", 0) == 0 && !codeFlag) {
                if (!blockText.empty()) {
                    rawBlock.push_back(blockText);
                    blockText.clear();
                }
            }
            blockText.push_back(curr);
            if (curr.rfind("```", 0) == 0) {
                if (codeFlag) {
                    rawBlock.push_back(blockText);
                    blockText.clear();
                }
                codeFlag = !codeFlag;
            }
            ins++;
            continue;
        }
        if (curr.size() >= 3 && std::isdigit(curr[0]) && curr[1] == '.' && curr[2] == ' ') {
            if (!orderedListsFlag) {
                if (!blockText.empty()) {
                    rawBlock.push_back(blockText);
                    blockText.clear();
                }
                orderedListsFlag = true;
            }
            blockText.push_back(curr);
            if (!next || *next == "\r" || *next == "\n" || next->empty())
                orderedListsFlag = false;
            ins++;
            continue;
        }
        if ((curr.size() >= 2 && curr[0] == '*' && curr[1] == ' ')
            || (curr.size() >= 2 && curr[0] == '+' && curr[1] == ' ')
            || (curr.size() >= 2 && curr[0] == '-' && curr[1] == ' ')) {
            if (!unorderedListFlag) {
                if (!blockText.empty()) {
                    rawBlock.push_back(blockText);
                    blockText.clear();
                }
                unorderedListFlag = true;
            }
            blockText.push_back(curr);
            if (!next || *next == "\r" || *next == "\n" || next->empty())
                unorderedListFlag = false;
            ins++;
            continue;
        }
        if (curr.size() >= 2 && curr[0] == '>' && curr[1] == ' ') {
            if (!blockQuoteFlag) {
                if (!blockText.empty()) {
                    rawBlock.push_back(blockText);
                    blockText.clear();
                }
                blockQuoteFlag = true;
            }
            blockText.push_back(curr);
            if (!next || *next == "\r" || *next == "\n" || next->empty())
                blockQuoteFlag = false;
            ins++;
            continue;
        }
        std::regex refRegex(R"(^\[([^\]]+)\]:\s*(.+)$)");
        std::smatch match;
        if (std::regex_match(curr, match, refRegex)) {
            std::string id = match[1].str();
            std::string url = match[2].str();
            refLinks[id] = url;
            ins++;
            continue;
        }
        if (next && !(*next == "\r" || *next == "\n" || next->empty())) {
            if (std::all_of(next->begin(), next->end() - 1, [](char c) { return c == '='; })
                && (next->back() == '\r' || next->back() == '\n' || next->back() == '=')) {
                if (!blockText.empty()) {
                    rawBlock.push_back(blockText);
                    blockText.clear();
                }
                blockText.push_back("# " + curr);
                rawBlock.push_back(blockText);
                blockText.clear();
                ins += 2;
                continue;
            }
            if (std::all_of(next->begin(), next->end() - 1, [](char c) { return c == '-'; })
                && (next->back() == '\r' || next->back() == '\n' || next->back() == '-')) {
                if (!blockText.empty()) {
                    rawBlock.push_back(blockText);
                    blockText.clear();
                }
                blockText.push_back("## " + curr);
                rawBlock.push_back(blockText);
                blockText.clear();
                ins += 2;
                continue;
            }
        }
        if ((curr.size() >= 2 && curr[0] == '#' && curr[1] == ' ')
            || (curr.size() >= 3 && curr[0] == '#' && curr[1] == '#' && curr[2] == ' ')
            || (curr.size() >= 4 && curr[0] == '#' && curr[1] == '#' && curr[2] == '#'
                && curr[3] == ' ')) {
            if (!blockText.empty()) {
                rawBlock.push_back(blockText);
                blockText.clear();
            }
            blockText.push_back(curr);
            rawBlock.push_back(blockText);
            blockText.clear();
            ins++;
            continue;
        }
        if (isHorizontalRules(curr, prev)) {
            if (!blockText.empty()) {
                rawBlock.push_back(blockText);
                blockText.clear();
            }
            blockText.push_back(curr);
            rawBlock.push_back(blockText);
            blockText.clear();
            ins++;
            continue;
        }
        if (!curr.empty()) {
            if (prev && (*prev == "\r" || *prev == "\n" || prev->empty())) {
                if (!blockText.empty()) {
                    rawBlock.push_back(blockText);
                    blockText.clear();
                }
            }
            blockText.push_back(curr);
            ins++;
            continue;
        }
        blockText.push_back(curr);
        ins++;
    }
    if (!blockText.empty()) {
        rawBlock.push_back(blockText);
    }
}

std::vector<MarkdownInlineElement> MarkdownParser::inlineParse(const std::string &rawText,
                                                               std::string &resText)
{
    std::string bufText = "";
    resText = "";
    std::vector<MarkdownInlineElement> resElem;

    bool space = false;
    for (size_t i = 0; i < rawText.size(); i++) {
        std::string token(1, rawText[i]);
        if (bufText.empty() && token == " ") {
            continue;
        }
        if (token == " ") {
            if (!space) {
                space = true;
            }
            continue;
        }
        if (token != " ") {
            if (space) {
                bufText += " ";
                space = false;
            }
            bufText += token;
        }
    }

    bool boldFlag = false;
    bool italicFlag = false;
    bool codeFlag = false;
    size_t begin = 0;
    size_t i = 0;
    size_t ins = 0;

    while (i < bufText.size()) {
        std::string token(1, bufText[i]);
        if (token == "!" && i + 1 < bufText.size() && bufText[i + 1] == '[') {
            size_t altStart = i + 2;
            size_t altEnd = bufText.find("]", altStart);
            if (altEnd != std::string::npos && altEnd + 1 < bufText.size()
                && bufText[altEnd + 1] == '(') {
                size_t urlStart = altEnd + 2;
                size_t urlEnd = bufText.find(")", urlStart);
                if (urlEnd != std::string::npos) {
                    std::string altText = bufText.substr(altStart, altEnd - altStart);
                    std::string url = bufText.substr(urlStart, urlEnd - urlStart);
                    resElem.push_back(MarkdownInlineElement(InlineType::Image, ins,
                                                            ins + altText.size(), url));
                    resText += altText;
                    i = urlEnd + 1;
                    ins += altText.size();
                    continue;
                }
            }
        }
        if (token == "[" && !italicFlag && !boldFlag && !codeFlag) {
            size_t textStart = i + 1;
            size_t textEnd = bufText.find("]", textStart);
            if (textEnd != std::string::npos && textEnd + 1 < bufText.size()
                && bufText[textEnd + 1] == '[') {
                size_t idStart = textEnd + 2;
                size_t idEnd = bufText.find("]", idStart);
                if (idEnd != std::string::npos) {
                    std::string linkText = bufText.substr(textStart, textEnd - textStart);
                    std::string id = bufText.substr(idStart, idEnd - idStart);
                    if (refLinks.count(id)) {
                        std::string url = refLinks[id];
                        resElem.push_back(MarkdownInlineElement(InlineType::Link, ins,
                                                                ins + linkText.size(), url));
                        resText += linkText;
                        i = idEnd + 1;
                        ins += linkText.size();
                        continue;
                    }
                }
            }
        }
        if (token == "[" && !italicFlag && !boldFlag && !codeFlag) {
            size_t textStart = i + 1;
            size_t textEnd = bufText.find("]", textStart);
            if (textEnd != std::string::npos && textEnd + 1 < bufText.size()
                && bufText[textEnd + 1] == '(') {
                size_t urlStart = textEnd + 2;
                size_t urlEnd = bufText.find(")", urlStart);
                if (urlEnd != std::string::npos) {
                    std::string linkText = bufText.substr(textStart, textEnd - textStart);
                    std::string urlFull = bufText.substr(urlStart, urlEnd - urlStart);
                    std::regex urlRegex(R"(https?://[^\s<]+)");
                    std::smatch urlMatch;
                    if (std::regex_search(urlFull, urlMatch, urlRegex)) {
                        std::string url = urlMatch.str();
                        resElem.push_back(MarkdownInlineElement(InlineType::Link, ins,
                                                                ins + linkText.size(), url));
                        resText += linkText;
                        i = urlEnd + 1;
                        ins += linkText.size();
                        continue;
                    }
                }
            }
        }
        if (token == "`" || codeFlag) {
            if (token == "`" && (!codeFlag)) {
                codeFlag = true;
                begin = ins;
                i++;
                continue;
            }
            if (token == "`" && codeFlag) {
                codeFlag = false;
                resElem.push_back(MarkdownInlineElement(InlineType::Code, begin, ins));
                i++;
                continue;
            }
        }

        if ((token == "*" || boldFlag || italicFlag) && !codeFlag) {
            if (token == "*" && (!boldFlag) && (!italicFlag)) {
                std::string tokenNext(1, bufText[i + 1]);
                if (tokenNext != "*") {
                    italicFlag = true;
                    begin = ins;
                    i++;
                    continue;
                } else {
                    boldFlag = true;
                    begin = ins;
                    i += 2;
                    continue;
                }
            }
            if (token == "*" && boldFlag && !italicFlag) {
                boldFlag = false;
                resElem.push_back(MarkdownInlineElement(InlineType::Bold, begin, ins));
                i += 2;
                continue;
            }
            if (token == "*" && italicFlag && !boldFlag) {
                italicFlag = false;
                resElem.push_back(MarkdownInlineElement(InlineType::Italic, begin, ins));
                i++;
                continue;
            }
        }
        resText += token;
        i++;
        ins++;
    }
    return resElem;
}

void MarkdownParser::blockParse(const std::string &rawText,
                                std::vector<MarkdownBlockElement> &blockElem)
{
    split(rawText);
    for (size_t i = 0; i < rawBlock.size(); i++) {
        BlockType type;
        std::string token = rawBlock[i][0].substr(0, 3);
        if (token == "```") {
            type = BlockType::CodeBlocks;
            std::vector<LineElement> lines;
            lines.push_back(LineElement(rawBlock[i][0].substr(3)));
            for (size_t j = 1; j < rawBlock[i].size() - 1; j++) {
                lines.push_back(LineElement(rawBlock[i][j]));
            }
            blockElem.push_back(MarkdownBlockElement(type, lines));
        } else if (token.size() >= 2 && token[0] == '1' && token[1] == '.') {
            type = BlockType::OrderedList;
            std::vector<LineElement> lines;
            for (const auto &line : rawBlock[i]) {
                std::string pureText;
                if (line.size() >= 2) {
                    std::vector<MarkdownInlineElement> inlineElem =
                            inlineParse(line.substr(2), pureText);
                    lines.push_back(LineElement(pureText, inlineElem));
                }
            }
            blockElem.push_back(MarkdownBlockElement(type, lines));
        } else if ((token.size() >= 2 && token.substr(0, 2) == "* ")
                   || (token.size() >= 2 && token.substr(0, 2) == "+ ")
                   || (token.size() >= 2 && token.substr(0, 2) == "- ")) {
            type = BlockType::UnorderedList;
            std::vector<LineElement> lines;
            for (const auto &line : rawBlock[i]) {
                std::string pureText;
                if (line.size() >= 2) {
                    std::vector<MarkdownInlineElement> inlineElem =
                            inlineParse(line.substr(2), pureText);
                    lines.push_back(LineElement(pureText, inlineElem));
                }
            }
            blockElem.push_back(MarkdownBlockElement(type, lines));
        } else if (token.size() >= 2 && token[0] == '>' && token[1] == ' ') {
            type = BlockType::BlockQuote;
            std::vector<LineElement> lines;
            for (const auto &line : rawBlock[i]) {
                std::string pureText;
                if (line.size() >= 2) {
                    std::vector<MarkdownInlineElement> inlineElem =
                            inlineParse(line.substr(2), pureText);
                    lines.push_back(LineElement(pureText, inlineElem));
                }
            }
            blockElem.push_back(MarkdownBlockElement(type, lines));
        } else if (token == "###") {
            type = BlockType::Headinglevel3;
            std::vector<LineElement> lines;
            std::string pureText;
            std::vector<MarkdownInlineElement> inlineElem =
                    inlineParse(rawBlock[i][0].substr(4), pureText);
            lines.push_back(LineElement(pureText, inlineElem));
            blockElem.push_back(MarkdownBlockElement(type, lines));
        } else if (token == "## ") {
            type = BlockType::Headinglevel2;
            std::vector<LineElement> lines;
            std::string pureText;
            std::vector<MarkdownInlineElement> inlineElem =
                    inlineParse(rawBlock[i][0].substr(3), pureText);
            lines.push_back(LineElement(pureText, inlineElem));
            blockElem.push_back(MarkdownBlockElement(type, lines));
        } else if (token.size() >= 1 && token[0] == '#') {
            type = BlockType::Headinglevel1;
            std::vector<LineElement> lines;
            std::string pureText;
            std::vector<MarkdownInlineElement> inlineElem =
                    inlineParse(rawBlock[i][0].substr(2), pureText);
            lines.push_back(LineElement(pureText, inlineElem));
            blockElem.push_back(MarkdownBlockElement(type, lines));
        } else if (isHorizontalRules(rawBlock[i][0], i ? &rawBlock[i - 1].back() : nullptr)) {
            type = BlockType::HorizontalRules;
            std::vector<LineElement> lines;
            lines.push_back(LineElement(""));
            blockElem.push_back(MarkdownBlockElement(type, lines));
        } else if (!token.empty()) {
            type = BlockType::Paragraph;
            std::vector<LineElement> lines;
            std::string pureText;
            for (size_t j = 0; j < rawBlock[i].size(); j++) {
                std::vector<MarkdownInlineElement> inlineElem =
                        inlineParse(rawBlock[i][j], pureText);
                lines.push_back(LineElement(pureText, inlineElem));
            }
            blockElem.push_back(MarkdownBlockElement(type, lines));
        }
    }
}

bool MarkdownParser::isHorizontalRules(const std::string &lineStr, const std::string *prevLine)
{
    if (lineStr.substr(0, 3) == "***"
        && std::all_of(lineStr.begin(), lineStr.end() - 1, [](char c) { return c == '*'; })
        && (lineStr.back() == '\r' || lineStr.back() == '\n' || lineStr.back() == '*'))
        return true;
    if (lineStr.substr(0, 3) == "___"
        && std::all_of(lineStr.begin(), lineStr.end() - 1, [](char c) { return c == '_'; })
        && (lineStr.back() == '\r' || lineStr.back() == '\n' || lineStr.back() == '_'))
        return true;
    if (!prevLine) {
        if (lineStr.substr(0, 3) == "---"
            && std::all_of(lineStr.begin(), lineStr.end() - 1, [](char c) { return c == '-'; })
            && (lineStr.back() == '\r' || lineStr.back() == '\n' || lineStr.back() == '-'))
            return true;
    } else if (*prevLine == "\r" || *prevLine == "\n" || prevLine->empty()) {
        if (lineStr.substr(0, 3) == "---"
            && std::all_of(lineStr.begin(), lineStr.end() - 1, [](char c) { return c == '-'; })
            && (lineStr.back() == '\r' || lineStr.back() == '\n' || lineStr.back() == '-'))
            return true;
    }
    return false;
}
