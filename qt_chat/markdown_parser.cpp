#include "markdown_parser.h"

#include <sstream>
#include <regex>
#include <iostream>
#include <QDebug>

#include <QtCore/QRegularExpression>
#include <QtCore/QRegularExpressionMatch>

void MarkdownParser::split(const QString &rawText)
{
    rawBlock.clear();
    std::istringstream iss(rawText.toStdString());
    std::vector<QString> rawLine;
    std::string lineText;
    while (std::getline(iss, lineText)) {
        rawLine.push_back(QString::fromStdString(lineText));
    }

    std::vector<QString> blockText;
    size_t ins = 0;
    bool codeFlag = false;
    bool orderedListsFlag = false;
    bool unorderedListFlag = false;
    bool blockQuoteFlag = false;

    while (ins < rawLine.size()) {
        const QString *prev = (ins > 0) ? &rawLine[ins - 1] : nullptr;
        const QString &curr = rawLine[ins];
        const QString *next = (ins + 1 < rawLine.size()) ? &rawLine[ins + 1] : nullptr;
        if (curr.isEmpty()) {
            blockText.push_back(curr);
            ins++;
            continue;
        }
        if (curr.indexOf("```", 0) == 0 || codeFlag) {
            if (curr.indexOf("```", 0) == 0 && !codeFlag) {
                if (!blockText.empty()) {
                    rawBlock.push_back(blockText);
                    blockText.clear();
                }
            }
            blockText.push_back(curr);
            if (curr.indexOf("```", 0) == 0) {
                if (codeFlag) {
                    rawBlock.push_back(blockText);
                    blockText.clear();
                }
                codeFlag = !codeFlag;
            }
            ins++;
            continue;
        }
        qDebug() << "markdown split curr:" << curr << curr.size();
        if (curr.size() >= 3 && curr[0].isDigit() && curr[1] == '.' && curr[2] == ' ') {
            if (!orderedListsFlag) {
                if (!blockText.empty()) {
                    rawBlock.push_back(blockText);
                    blockText.clear();
                }
                orderedListsFlag = true;
            }
            blockText.push_back(curr);
            if (!next || *next == "\r" || *next == "\n" || next->isEmpty())
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
            if (!next || *next == "\r" || *next == "\n" || next->isEmpty())
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
            if (!next || *next == "\r" || *next == "\n" || next->isEmpty())
                blockQuoteFlag = false;
            ins++;
            continue;
        }
        // std::regex refRegex(R"(^\[([^\]]+)\]:\s*(.+)$)");
        // std::smatch match;
        QRegularExpression refRegex(R"(^\[([^\]]+)\]:\s*(.+)$)");
        QRegularExpressionMatch match = refRegex.match(curr);
        if (match.hasMatch()) {
            // QString id = match[1].str();
            // QString url = match[2].str();
            QString id = match.captured(1);
            QString url = match.captured(2);
            refLinks[id] = url;
            ins++;
            continue;
        }
        if (next && !(*next == "\r" || *next == "\n" || next->isEmpty())) {
            if (std::all_of(next->begin(), next->end() - 1, [](QChar c) { return c == '='; })
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
            if (std::all_of(next->begin(), next->end() - 1, [](QChar c) { return c == '-'; })
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
        if (!curr.isEmpty()) {
            if (prev && (*prev == "\r" || *prev == "\n" || prev->isEmpty())) {
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

std::vector<MarkdownInlineElement> MarkdownParser::inlineParse(const QString &rawText,
                                                               QString &resText)
{
    QString bufText = "";
    resText = "";
    std::vector<MarkdownInlineElement> resElem;

    bool space = false;
    for (int i = 0; i < rawText.size(); i++) {
        QString token(1, rawText[i]);
        if (bufText.isEmpty() && token == " ") {
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
    int i = 0;
    size_t ins = 0;

    while (i < bufText.size()) {
        QString token(1, bufText[i]);
        if (token == "!" && i + 1 < bufText.size() && bufText[i + 1] == '[') {
            size_t altStart = i + 2;
            int altEnd = bufText.indexOf("]", altStart);
            if (altEnd != -1 && altEnd + 1 < bufText.size()
                && bufText[altEnd + 1] == '(') {
                size_t urlStart = altEnd + 2;
                int urlEnd = bufText.indexOf(")", urlStart);
                if (urlEnd != -1) {
                    QString altText = bufText.mid(altStart, altEnd - altStart);
                    QString url = bufText.mid(urlStart, urlEnd - urlStart);
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
            int textEnd = bufText.indexOf("]", textStart);
            if (textEnd != -1 && textEnd + 1 < bufText.size()
                && bufText[textEnd + 1] == '[') {
                size_t idStart = textEnd + 2;
                int idEnd = bufText.indexOf("]", idStart);
                if (idEnd != -1) {
                    QString linkText = bufText.mid(textStart, textEnd - textStart);
                    QString id = bufText.mid(idStart, idEnd - idStart);
                    if (refLinks.count(id)) {
                        QString url = refLinks[id];
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
            int textEnd = bufText.indexOf("]", textStart);
            if (textEnd != -1 && textEnd + 1 < bufText.size()
                && bufText[textEnd + 1] == '(') {
                size_t urlStart = textEnd + 2;
                int urlEnd = bufText.indexOf(")", urlStart);
                if (urlEnd != -1) {
                    QString linkText = bufText.mid(textStart, textEnd - textStart);
                    QString urlFull = bufText.mid(urlStart, urlEnd - urlStart);
                    // std::regex urlRegex(R"(https?://[^\s<]+)");
                    // std::smatch urlMatch;
                    QRegularExpression urlRegex(R"(https?://[^\s<]+)");
                    QRegularExpressionMatch urlMatch = urlRegex.match(urlFull);
                    if (urlMatch.hasMatch()) {
                        // QString url = urlMatch.str();
                        QString url = urlMatch.captured(0);
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
                QString tokenNext(1, bufText[i + 1]);
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

void MarkdownParser::blockParse(const QString &rawText,
                                std::vector<MarkdownBlockElement> &blockElem)
{
    split(rawText);
    for (size_t i = 0; i < rawBlock.size(); i++) {
        BlockType type;
        QString token = rawBlock[i][0].mid(0, 3);
        QString token1 = "";
        if (rawBlock[i][0].size() > 3)
            token1 = rawBlock[i][0].mid(3, 2);
        if (token == "```") {
            type = BlockType::CodeBlocks;
            std::vector<LineElement> lines;
            lines.push_back(LineElement(rawBlock[i][0].mid(3)));
            for (size_t j = 1; j < rawBlock[i].size() - 1; j++) {
                lines.push_back(LineElement(rawBlock[i][j]));
            }
            blockElem.push_back(MarkdownBlockElement(type, lines));
        } else if (token.size() >= 2 && token[0] == '1' && token[1] == '.') {
            type = BlockType::OrderedList;
            std::vector<LineElement> lines;
            for (const auto &line : rawBlock[i]) {
                QString pureText;
                if (line.size() >= 2) {
                    std::vector<MarkdownInlineElement> inlineElem =
                            inlineParse(line.mid(2), pureText);
                    lines.push_back(LineElement(pureText, inlineElem));
                }
            }
            blockElem.push_back(MarkdownBlockElement(type, lines));
        } else if ((token.size() >= 2 && token.mid(0, 2) == "* ")
                   || (token.size() >= 2 && token.mid(0, 2) == "+ ")
                   || (token.size() >= 2 && token.mid(0, 2) == "- ")) {
            type = BlockType::UnorderedList;
            std::vector<LineElement> lines;
            for (const auto &line : rawBlock[i]) {
                QString pureText;
                if (line.size() >= 2) {
                    std::vector<MarkdownInlineElement> inlineElem =
                            inlineParse(line.mid(2), pureText);
                    lines.push_back(LineElement(pureText, inlineElem));
                }
            }
            blockElem.push_back(MarkdownBlockElement(type, lines));
        } else if (token.size() >= 2 && token[0] == '>' && token[1] == ' ') {
            type = BlockType::BlockQuote;
            std::vector<LineElement> lines;
            for (const auto &line : rawBlock[i]) {
                QString pureText;
                if (line.size() >= 2) {
                    std::vector<MarkdownInlineElement> inlineElem =
                            inlineParse(line.mid(2), pureText);
                    lines.push_back(LineElement(pureText, inlineElem));
                }
            }
            blockElem.push_back(MarkdownBlockElement(type, lines));
        } else if (token == "###" && token1.size() > 1 && token1[0] == ' ') {
            type = BlockType::Headinglevel3;
            std::vector<LineElement> lines;
            QString pureText;
            std::vector<MarkdownInlineElement> inlineElem =
                    inlineParse(rawBlock[i][0].mid(4), pureText);
            lines.push_back(LineElement(pureText, inlineElem));
            blockElem.push_back(MarkdownBlockElement(type, lines));
        } else if (token == "## " && token1.size() > 0) {
            type = BlockType::Headinglevel2;
            std::vector<LineElement> lines;
            QString pureText;
            std::vector<MarkdownInlineElement> inlineElem =
                    inlineParse(rawBlock[i][0].mid(3), pureText);
            lines.push_back(LineElement(pureText, inlineElem));
            blockElem.push_back(MarkdownBlockElement(type, lines));
        } else if (token.size() > 2 && token[0] == '#' && token[1] == ' ') {
            type = BlockType::Headinglevel1;
            std::vector<LineElement> lines;
            QString pureText;
            std::vector<MarkdownInlineElement> inlineElem =
                    inlineParse(rawBlock[i][0].mid(2), pureText);
            lines.push_back(LineElement(pureText, inlineElem));
            blockElem.push_back(MarkdownBlockElement(type, lines));
        } else if (isHorizontalRules(rawBlock[i][0], i ? &rawBlock[i - 1].back() : nullptr)) {
            type = BlockType::HorizontalRules;
            std::vector<LineElement> lines;
            lines.push_back(LineElement(""));
            blockElem.push_back(MarkdownBlockElement(type, lines));
        } else if (!token.isEmpty()) {
            type = BlockType::Paragraph;
            std::vector<LineElement> lines;
            QString pureText;
            for (size_t j = 0; j < rawBlock[i].size(); j++) {
                std::vector<MarkdownInlineElement> inlineElem =
                        inlineParse(rawBlock[i][j], pureText);
                lines.push_back(LineElement(pureText, inlineElem));
            }
            blockElem.push_back(MarkdownBlockElement(type, lines));
        }
    }
}

bool MarkdownParser::isHorizontalRules(const QString &lineStr, const QString *prevLine)
{
    if (lineStr.mid(0, 3) == "***"
        && std::all_of(lineStr.begin(), lineStr.end() - 1, [](QChar c) { return c == '*'; })
        && (lineStr.back() == '\r' || lineStr.back() == '\n' || lineStr.back() == '*'))
        return true;
    if (lineStr.mid(0, 3) == "___"
        && std::all_of(lineStr.begin(), lineStr.end() - 1, [](QChar c) { return c == '_'; })
        && (lineStr.back() == '\r' || lineStr.back() == '\n' || lineStr.back() == '_'))
        return true;
    if (!prevLine) {
        if (lineStr.mid(0, 3) == "---"
            && std::all_of(lineStr.begin(), lineStr.end() - 1, [](QChar c) { return c == '-'; })
            && (lineStr.back() == '\r' || lineStr.back() == '\n' || lineStr.back() == '-'))
            return true;
    } else if (*prevLine == "\r" || *prevLine == "\n" || prevLine->isEmpty()) {
        if (lineStr.mid(0, 3) == "---"
            && std::all_of(lineStr.begin(), lineStr.end() - 1, [](QChar c) { return c == '-'; })
            && (lineStr.back() == '\r' || lineStr.back() == '\n' || lineStr.back() == '-'))
            return true;
    }
    return false;
}
