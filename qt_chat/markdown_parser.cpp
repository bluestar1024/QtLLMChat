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

// 行首 ATX 标题层级："#"（1-6 个）后跟空格时返回层级（1-6），否则返回 0。
// split 与 blockParse 共用此判定：此前只识别 1-3 级标题，
// "#### xxx" 等 4-6 级标题不匹配任何分支，被当作普通段落渲染出字面 "#"
static int headingLevelOf(const QString &line)
{
    int level = 0;
    while (level < line.size() && level < 6 && line[level] == '#')
        ++level;
    if (level == 0 || level >= line.size() || line[level] != ' ')
        return 0;
    return level;
}

// 行首列表标记识别：有序（"数字." 后跟空格或行尾，数字可为多位数，如 "10."）
// 或无序（"-"/"*"/"+" 后跟空格）；命中时返回正文起点（跳过标记与其后空格），
// 未命中返回 -1。leadOut 输出前导空格数，orderedOut 输出是否有序标记。
// maxLead 限制前导空格数：普通行 3（与 stripLeadSpaces 一致）；列表块内续行
// 放宽（"1. " 的子项缩进 3 空格、"10. " 的子项缩进 4 空格、嵌套更深同理）
static int listMarkEnd(const QString &line, int maxLead, int *leadOut = nullptr,
                       bool *orderedOut = nullptr)
{
    int p = 0;
    while (p < line.size() && p < maxLead && line[p] == ' ')
        ++p;
    // 前导空格超出上限（仍停在空格上）时不视为列表标记
    if (p == maxLead && p < line.size() && line[p] == ' ')
        return -1;
    int numEnd = p;
    while (numEnd < line.size() && line[numEnd].isDigit())
        ++numEnd;
    if (numEnd > p && numEnd < line.size() && line[numEnd] == '.'
        && (numEnd + 1 == line.size() || line[numEnd + 1] == ' ')) {
        if (leadOut)
            *leadOut = p;
        if (orderedOut)
            *orderedOut = true;
        return numEnd + 1 == line.size() ? numEnd + 1 : numEnd + 2;
    }
    if (p + 1 < line.size() && (line[p] == '-' || line[p] == '*' || line[p] == '+')
        && line[p + 1] == ' ') {
        if (leadOut)
            *leadOut = p;
        if (orderedOut)
            *orderedOut = false;
        return p + 2;
    }
    return -1;
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
        // 列表行（列表项/缩进子项）统一处理：列表块持续到出现非列表行为止。
        // 此前"列表项后跟空行立即结束列表"，使 1./2./3. 各成独立 <ol>，
        // 浏览器对每个 <ol> 从 1 重新编号，整段渲染成 "1. 1. 1."；
        // 缩进子项行也不匹配旧的行首标记判定，导致后续列表项被误并入同块
        const bool inList = orderedListsFlag || unorderedListFlag;
        int listLead = 0;
        bool listOrdered = false;
        // 块内续行放宽缩进上限：兼容 "1. " 的 3 空格与 "10. " 的 4 空格子项
        const int listMark = listMarkEnd(curr, inList ? 8 : 3, &listLead, &listOrdered);
        if (listMark >= 0) {
            // 顶层的异类标记（如有序列表后紧跟 "- "）开启新列表：子项/新列表
            // 若混入原块会占用有序编号，导致后续列表项编号错位
            if (!inList || (listLead == 0 && listOrdered != orderedListsFlag)) {
                if (!blockText.empty()) {
                    rawBlock.push_back(blockText);
                    blockText.clear();
                }
                orderedListsFlag = listOrdered;
                unorderedListFlag = !listOrdered;
            }
            blockText.push_back(curr);
            ins++;
            continue;
        }
        // 非列表行到来，列表块结束；空行不断开列表（见上）
        if (inList) {
            orderedListsFlag = false;
            unorderedListFlag = false;
            if (!blockText.empty()) {
                rawBlock.push_back(blockText);
                blockText.clear();
            }
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
        // 标题行独立成块；判定与 blockParse 一致（同一辅助函数），
        // 并覆盖 4-6 级标题
        if (headingLevelOf(stripLeadSpaces(curr)) > 0) {
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
        int headLevel = headingLevelOf(head);
        // 列表块首行标记（支持多位数有序序号 "10."）；新块开启条件与 split 一致取 maxLead = 3
        bool headOrdered = false;
        const int headMarkEnd = listMarkEnd(rawBlock[i][0], 3, nullptr, &headOrdered);
        if (token == "```") {
            type = BlockType::CodeBlocks;
            std::vector<LineElement> lines;
            lines.push_back(LineElement(rawBlock[i][0].mid(3)));
            for (size_t j = 1; j < rawBlock[i].size() - 1; j++) {
                lines.push_back(LineElement(rawBlock[i][j]));
            }
            blockElem.push_back(MarkdownBlockElement(type, lines));
        } else if (headMarkEnd >= 0) {
            // 列表块（有序/无序由块首行标记决定）：逐行剥离任意列表标记
            // （块内子项标记可与块类型不同，如有序列表下的 "- "），并按相对
            // 缩进推导嵌套级别，供渲染器生成嵌套 <ul>/<ol>
            type = headOrdered ? BlockType::OrderedList : BlockType::UnorderedList;
            std::vector<LineElement> lines;
            std::vector<int> indentStack;
            for (const auto &line : rawBlock[i]) {
                if (line.isEmpty())
                    continue;
                int lead = 0;
                bool ordered = false;
                // 块内行沿用放宽的缩进上限（与 split 判定一致）
                const int mark = listMarkEnd(line, 8, &lead, &ordered);
                QString body;
                if (mark >= 0) {
                    body = line.mid(mark);
                    // 相对缩进层级：缩进变浅弹栈、变深压栈；子项缩进宽度不定
                    // （"1. " 为 3、"10. " 为 4 等），以父级缩进为基准最稳
                    while (indentStack.size() > 1 && lead < indentStack.back())
                        indentStack.pop_back();
                    if (indentStack.empty() || lead > indentStack.back())
                        indentStack.push_back(lead);
                } else {
                    // 非列表行（如游离文本）原样解析，避免丢失行首字符
                    body = line;
                }
                QString pureText;
                std::vector<MarkdownInlineElement> inlineElem = inlineParse(body, pureText);
                lines.push_back(LineElement(pureText, inlineElem,
                                            indentStack.empty() ? 0 : int(indentStack.size()) - 1,
                                            ordered));
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
        } else if (headLevel > 0) {
            // 1-6 级 ATX 标题统一处理：层级决定渲染标签，正文为
            // "标记 + 空格" 之后的内容（headLevel + 1 个字符）
            switch (headLevel) {
            case 1:
                type = BlockType::Headinglevel1;
                break;
            case 2:
                type = BlockType::Headinglevel2;
                break;
            case 3:
                type = BlockType::Headinglevel3;
                break;
            case 4:
                type = BlockType::Headinglevel4;
                break;
            case 5:
                type = BlockType::Headinglevel5;
                break;
            default:
                type = BlockType::Headinglevel6;
                break;
            }
            std::vector<LineElement> lines;
            QString pureText;
            std::vector<MarkdownInlineElement> inlineElem =
                    inlineParse(head.mid(headLevel + 1), pureText);
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
