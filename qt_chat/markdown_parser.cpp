#include <sstream>
#include <regex>
#include <iostream>
#include "markdown_parser.h"

void Markdown_Parser::split(const std::string& RawText) {
    RawBlock.clear();
    std::istringstream iss(RawText);
    std::vector<std::string> RawLine;
    std::string LineText;
    while (std::getline(iss, LineText)) {
        RawLine.push_back(LineText);
    }

    std::vector<std::string> BlockText;
    size_t ins = 0;
    bool Code_flag = false;
    bool OrderedLists_flag = false;
    bool UnorderedList_flag = false;
    bool BlockQuote_flag = false;

    while(ins < RawLine.size()) {
        const std::string* prev = (ins > 0) ? &RawLine[ins - 1] : nullptr;
        const std::string& curr = RawLine[ins];
        const std::string* next = (ins + 1 < RawLine.size()) ? &RawLine[ins + 1] : nullptr;
        // std::cout << "test\t" << curr << '\n';
        if (curr.empty()) {
            BlockText.push_back(curr);
            ins++;
            continue;
        }
        // 代码块
        if (curr.rfind("```", 0) == 0 || Code_flag) {
            if (curr.rfind("```", 0) == 0 && !Code_flag) {
                if (!BlockText.empty()) {
                    RawBlock.push_back(BlockText);
                    BlockText.clear();
                }
            }
            BlockText.push_back(curr);
            if (curr.rfind("```", 0) == 0) {
                if (Code_flag) {
                    RawBlock.push_back(BlockText);
                    BlockText.clear();
                }
                Code_flag = !Code_flag;
            }
            ins++;
            continue;
        }

        // 有序列表
        if (curr.size() >= 3 && std::isdigit(curr[0]) && curr[1] == '.' && curr[2] == ' ') {
            if (!OrderedLists_flag) {
                if (!BlockText.empty()) {
                    RawBlock.push_back(BlockText);
                    BlockText.clear();
                }
                OrderedLists_flag = true;
            }
            BlockText.push_back(curr);
            if (!next || *next == "\r" || *next == "\n" || next->empty())
                OrderedLists_flag = false;
            ins++;
            continue;
        }
        // 无序列表
        if (curr.size() >= 2 && curr[0] == '-' && curr[1] == ' ') {
            if (!UnorderedList_flag) {
                if (!BlockText.empty()) {
                    RawBlock.push_back(BlockText);
                    BlockText.clear();
                }
                UnorderedList_flag = true;
            }
            BlockText.push_back(curr);
            if (!next || *next == "\r" || *next == "\n" || next->empty())
                UnorderedList_flag = false;
            ins++;
            continue;
        }
        // 引用
        if (curr.size() >= 2 && curr[0] == '>' && curr[1] == ' ') {
            if (!BlockQuote_flag) {
                if (!BlockText.empty()) {
                    RawBlock.push_back(BlockText);
                    BlockText.clear();
                }
                BlockQuote_flag = true;
            }
            BlockText.push_back(curr);
            if (!next || *next == "\r" || *next == "\n" || next->empty())
                BlockQuote_flag = false;
            ins++;
            continue;
        }
        // 标题
        if ((curr.size() >= 2 && curr[0] == '#' && curr[1] == ' ') ||
            (curr.size() >= 3 && curr[0] == '#' && curr[1] == '#' && curr[2] == ' ') ||
            (curr.size() >= 4 && curr[0] == '#' && curr[1] == '#' && curr[2] == '#' && curr[3] == ' ')) {
            if (!BlockText.empty()) {
                RawBlock.push_back(BlockText);
                BlockText.clear();
            }
            BlockText.push_back(curr);
            RawBlock.push_back(BlockText);
            BlockText.clear();
            ins++;
            continue;
        }
        // 分割线
        if (isHorizontalRules(curr)) {
            if (!BlockText.empty()) {
                RawBlock.push_back(BlockText);
                BlockText.clear();
            }
            BlockText.push_back(curr);
            RawBlock.push_back(BlockText);
            BlockText.clear();
            ins++;
            continue;
        }
        // 正文
        if (!curr.empty()) {
            if (prev && (*prev == "\r" || *prev == "\n" || prev->empty())) {
                if (!BlockText.empty()) {
                    RawBlock.push_back(BlockText);
                    BlockText.clear();
                }
            }
            BlockText.push_back(curr);
            ins++;
            continue;
        }
        // 空行
        BlockText.push_back(curr);
        ins++;
    }
    if (!BlockText.empty()) {
        RawBlock.push_back(BlockText);
    }
}

std::vector<Markdown_InlineElement> Markdown_Parser::inline_parse(const std::string& RawText, std::string& ResText) {
    std::string BufText = "";
    ResText = "";
    std::vector<Markdown_InlineElement> ResElem;

    bool space = false;
    for (size_t i = 0; i < RawText.size(); i++) {
        std::string token(1, RawText[i]);
        if (BufText.empty() && token == " ") { continue; }
        if (token == " ") {
            if (!space) { space = true; }
            continue;
        }
        if (token != " ") {
            if (space) { BufText += " "; space = false; }
            BufText += token;
        }
    }

    bool Bold_flag = false;
    bool Italic_flag = false;
    bool Code_flag = false;
    size_t begin = 0;
    size_t i = 0;
    size_t ins = 0;

    while (i < BufText.size()) {
        std::string token(1, BufText[i]);
        if (token == "!" && i + 1 < BufText.size() && BufText[i + 1] == '[') {
            size_t alt_start = i + 2;
            size_t alt_end = BufText.find("]", alt_start);
            if (alt_end != std::string::npos && alt_end + 1 < BufText.size() && BufText[alt_end + 1] == '(') {
                size_t url_start = alt_end + 2;
                size_t url_end = BufText.find(")", url_start);
                if (url_end != std::string::npos) {
                    std::string alt_text = BufText.substr(alt_start, alt_end - alt_start);
                    std::string url = BufText.substr(url_start, url_end - url_start);

                    ResElem.push_back(Markdown_InlineElement(InlineType::Image, ins, ins + alt_text.size(), url));
                    ResText += alt_text;

                    i = url_end + 1;
                    ins += alt_text.size();
                    continue;
                }
            }
        }
        if (token == "`" || Code_flag) {
            if (token == "`" && (!Code_flag)) {
                Code_flag = true;
                begin = ins;
                i++;
                continue;
            }
            if (token == "`" && Code_flag) {
                Code_flag = false;
                ResElem.push_back(Markdown_InlineElement(InlineType::Code, begin, ins));
                i++;
                continue;
            }
        }

        if ((token == "*" || Bold_flag || Italic_flag) && !Code_flag) {
            if (token == "*" && (!Bold_flag) && (!Italic_flag)) {
                std::string token_next(1, BufText[i + 1]);
                if (token_next != "*") {
                    Italic_flag = true;
                    begin = ins;
                    i++; continue;
                }
                else {
                    Bold_flag = true;
                    begin = ins;
                    i += 2; continue;
                }
            }
            if(token == "*" && Bold_flag && !Italic_flag) {
                Bold_flag = false;
                ResElem.push_back(Markdown_InlineElement(InlineType::Bold, begin, ins));
                i += 2; continue;
            }
            if(token == "*" && Italic_flag && !Bold_flag) {
                Italic_flag = false;
                ResElem.push_back(Markdown_InlineElement(InlineType::Italic, begin, ins));
                i++; continue;
            }
        }
        ResText += token;
        i++;
        ins++;
    }
    return ResElem;
}

void Markdown_Parser::block_parse(const std::string& RawText, std::vector<Markdown_BlockElement>& BlockElem) {
    split(RawText);
    for (size_t i = 0; i < RawBlock.size(); i++) {
        BlockType type;
        std::string token = RawBlock[i][0].substr(0, 3);
        if (token == "```") {
            type = BlockType::CodeBlocks;
            std::vector<LineElement> Lines;
            Lines.push_back(LineElement(RawBlock[i][0].substr(3)));
            for (size_t j = 1; j < RawBlock[i].size() - 1; j++) {
                Lines.push_back(LineElement(RawBlock[i][j]));
            }
            BlockElem.push_back(Markdown_BlockElement(type, Lines));
        }
        else if (token.size() >= 2 && token[0] == '1' && token[1] == '.') {
            type = BlockType::OrderedList;
            std::vector<LineElement> Lines;
            for (const auto& line : RawBlock[i]) {
                std::string pure_text;
                if (line.size() >= 2) {
                    std::vector<Markdown_InlineElement> InlineElem = inline_parse(line.substr(2), pure_text);
                    Lines.push_back(LineElement(pure_text, InlineElem));
                }
            }
            BlockElem.push_back(Markdown_BlockElement(type, Lines));
        }
        else if (token.size() >= 2 && token.substr(0, 2) == "- ") {
            type = BlockType::UnorderedList;
            std::vector<LineElement> Lines;
            for (const auto& line : RawBlock[i]) {
                std::string pure_text;
                if (line.size() >= 2) {
                    std::vector<Markdown_InlineElement> InlineElem = inline_parse(line.substr(2), pure_text);
                    Lines.push_back(LineElement(pure_text, InlineElem));
                }
            }
            BlockElem.push_back(Markdown_BlockElement(type, Lines));
        }
        else if (token.size() >= 2 && token[0] == '>' && token[1] == ' ') {
            type = BlockType::BlockQuote;
            std::vector<LineElement> Lines;
            for (const auto& line : RawBlock[i]) {
                std::string pure_text;
                if (line.size() >= 2) {
                    std::vector<Markdown_InlineElement> InlineElem = inline_parse(line.substr(2), pure_text);
                    Lines.push_back(LineElement(pure_text, InlineElem));
                }
            }
            BlockElem.push_back(Markdown_BlockElement(type, Lines));
        }
        else if (token == "###") {
            type = BlockType::Headinglevel3;
            std::vector<LineElement> Lines;
            std::string pure_text;
            std::vector<Markdown_InlineElement> InlineElem = inline_parse(RawBlock[i][0].substr(4), pure_text);
            Lines.push_back(LineElement(pure_text, InlineElem));
            BlockElem.push_back(Markdown_BlockElement(type, Lines));
        }
        else if (token == "## ") {
            type = BlockType::Headinglevel2;
            std::vector<LineElement> Lines;
            std::string pure_text;
            std::vector<Markdown_InlineElement> InlineElem = inline_parse(RawBlock[i][0].substr(3), pure_text);
            Lines.push_back(LineElement(pure_text, InlineElem));
            BlockElem.push_back(Markdown_BlockElement(type, Lines));
        }
        else if (token.size() >= 1 && token[0] == '#') {
            type = BlockType::Headinglevel1;
            std::vector<LineElement> Lines;
            std::string pure_text;
            std::vector<Markdown_InlineElement> InlineElem = inline_parse(RawBlock[i][0].substr(2), pure_text);
            Lines.push_back(LineElement(pure_text, InlineElem));
            BlockElem.push_back(Markdown_BlockElement(type, Lines));
        }
        else if (isHorizontalRules(RawBlock[i][0])) {
            type = BlockType::HorizontalRules;
            std::vector<LineElement> Lines;
            Lines.push_back(LineElement(""));
            BlockElem.push_back(Markdown_BlockElement(type, Lines));
        }
        else if (!token.empty()) {
            type = BlockType::Paragraph;
            std::vector<LineElement> Lines;
            std::string pure_text;
            for(size_t j = 0; j < RawBlock[i].size(); j++)
            {
                std::vector<Markdown_InlineElement> InlineElem = inline_parse(RawBlock[i][j], pure_text);
                Lines.push_back(LineElement(pure_text, InlineElem));
            }
            BlockElem.push_back(Markdown_BlockElement(type, Lines));
        }
    }
}

bool Markdown_Parser::isHorizontalRules(const std::string& lineStr) {
    if (lineStr.substr(0, 3) == "---" &&
        std::all_of(lineStr.begin(), lineStr.end() - 1, [](char c) { return c == '-'; }) &&
        (lineStr.back() == '\r' || lineStr.back() == '\n' || lineStr.back() == '-'))
        return true;
    if (lineStr.substr(0, 3) == "***" &&
        std::all_of(lineStr.begin(), lineStr.end() - 1, [](char c) { return c == '*'; }) &&
        (lineStr.back() == '\r' || lineStr.back() == '\n' || lineStr.back() == '*'))
        return true;
    if (lineStr.substr(0, 3) == "___" &&
        std::all_of(lineStr.begin(), lineStr.end() - 1, [](char c) { return c == '_'; }) &&
        (lineStr.back() == '\r' || lineStr.back() == '\n' || lineStr.back() == '_'))
        return true;
    return false;
}
