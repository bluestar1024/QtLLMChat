#include "markdown_parser.h"

#include <sstream>
#include <regex>
#include <iostream>
#include <QDebug>

#include <QtCore/QRegularExpression>
#include <QtCore/QRegularExpressionMatch>

// 行首至多 3 个空格属于缩进（AI 输出的列表项常带三个空格缩进）：
// 去掉缩进用于块类型识别，避免 "   - xxx" 落入普通段落分支渲染出字面 "- "
static QString stripLeadSpaces(const QString &line)
{
    int lead = 0;
    while (lead < line.size() && lead < 3 && line[lead] == ' ')
        ++lead;
    if (lead == 0)
        return line;
    if (lead < line.size() && line[lead] == ' ')
        return line;
    QString stripped = line.mid(lead);
    // 全空白行与代码围栏行保持原样，防止误判为其它块或丢失空行
    if (stripped.isEmpty() || stripped.startsWith("```"))
        return line;
    return stripped;
}

// 剥离有序列表标记（"数字. "，允许至多 3 个前导空格）；未匹配时返回原行
static QString stripOrderedMark(const QString &line)
{
    int p = 0;
    while (p < line.size() && p < 3 && line[p] == ' ')
        ++p;
    int numEnd = p;
    while (numEnd < line.size() && line[numEnd].isDigit())
        ++numEnd;
    if (numEnd > p && numEnd + 1 < line.size() && line[numEnd] == '.'
        && line[numEnd + 1] == ' ') {
        return line.mid(numEnd + 2);
    }
    return line;
}

// 剥离无序列表标记（"- "、"* "、"+ "，允许至多 3 个前导空格）；未匹配时返回原行
static QString stripUnorderedMark(const QString &line)
{
    int p = 0;
    while (p < line.size() && p < 3 && line[p] == ' ')
        ++p;
    if (p + 1 < line.size() && (line[p] == '-' || line[p] == '*' || line[p] == '+')
        && line[p + 1] == ' ') {
        return line.mid(p + 2);
    }
    return line;
}

// 剥离引用标记（"> "，允许至多 3 个前导空格）；未匹配时返回原行
static QString stripQuoteMark(const QString &line)
{
    int p = 0;
    while (p < line.size() && p < 3 && line[p] == ' ')
        ++p;
    if (p + 1 < line.size() && line[p] == '>' && line[p + 1] == ' ') {
        return line.mid(p + 2);
    }
    return line;
}

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
        // 行首至多 3 个空格视为缩进，识别块类型前先剥离
        QString head = stripLeadSpaces(rawBlock[i][0]);
        QString token = head.mid(0, 3);
        QString token1 = "";
        if (head.size() > 3)
            token1 = head.mid(3, 2);
        if (token == "```") {
            type = BlockType::CodeBlocks;
            std::vector<LineElement> lines;
            lines.push_back(LineElement(rawBlock[i][0].mid(3)));
            for (size_t j = 1; j < rawBlock[i].size() - 1; j++) {
                lines.push_back(LineElement(rawBlock[i][j]));
            }
            blockElem.push_back(MarkdownBlockElement(type, lines));
        } else if (token.size() >= 2 && token[0].isDigit() && token[1] == '.'
                   && (token.size() == 2 || token[2] == ' ')) {
            type = BlockType::OrderedList;
            std::vector<LineElement> lines;
            for (const auto &line : rawBlock[i]) {
                if (line.isEmpty())
                    continue;
                QString pureText;
                // 逐行校验并剥离 "数字. " 标记；非列表行（如游离文本）原样解析，
                // 避免无差别 mid(2) 丢失行首字符
                QString body = stripOrderedMark(line);
                std::vector<MarkdownInlineElement> inlineElem = inlineParse(body, pureText);
                lines.push_back(LineElement(pureText, inlineElem));
            }
            blockElem.push_back(MarkdownBlockElement(type, lines));
        } else if ((token.size() >= 2 && token.mid(0, 2) == "* ")
                   || (token.size() >= 2 && token.mid(0, 2) == "+ ")
                   || (token.size() >= 2 && token.mid(0, 2) == "- ")) {
            type = BlockType::UnorderedList;
            std::vector<LineElement> lines;
            for (const auto &line : rawBlock[i]) {
                if (line.isEmpty())
                    continue;
                QString pureText;
                QString body = stripUnorderedMark(line);
                std::vector<MarkdownInlineElement> inlineElem = inlineParse(body, pureText);
                lines.push_back(LineElement(pureText, inlineElem));
            }
            blockElem.push_back(MarkdownBlockElement(type, lines));
        } else if (token.size() >= 2 && token[0] == '>' && token[1] == ' ') {
            type = BlockType::BlockQuote;
            std::vector<LineElement> lines;
            for (const auto &line : rawBlock[i]) {
                if (line.isEmpty())
                    continue;
                QString pureText;
                QString body = stripQuoteMark(line);
                std::vector<MarkdownInlineElement> inlineElem = inlineParse(body, pureText);
                lines.push_back(LineElement(pureText, inlineElem));
            }
            blockElem.push_back(MarkdownBlockElement(type, lines));
        } else if (token == "###" && token1.size() >= 1 && token1[0] == ' ') {
            type = BlockType::Headinglevel3;
            std::vector<LineElement> lines;
            QString pureText;
            std::vector<MarkdownInlineElement> inlineElem =
                    inlineParse(head.mid(4), pureText);
            lines.push_back(LineElement(pureText, inlineElem));
            blockElem.push_back(MarkdownBlockElement(type, lines));
        } else if (token == "## ") {
            type = BlockType::Headinglevel2;
            std::vector<LineElement> lines;
            QString pureText;
            std::vector<MarkdownInlineElement> inlineElem =
                    inlineParse(head.mid(3), pureText);
            lines.push_back(LineElement(pureText, inlineElem));
            blockElem.push_back(MarkdownBlockElement(type, lines));
        } else if (token.size() >= 2 && token[0] == '#' && token[1] == ' ') {
            type = BlockType::Headinglevel1;
            std::vector<LineElement> lines;
            QString pureText;
            std::vector<MarkdownInlineElement> inlineElem =
                    inlineParse(head.mid(2), pureText);
            lines.push_back(LineElement(pureText, inlineElem));
            blockElem.push_back(MarkdownBlockElement(type, lines));
        } else if (isHorizontalRules(head, i ? &rawBlock[i - 1].back() : nullptr)) {
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
