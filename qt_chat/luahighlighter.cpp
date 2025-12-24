#include "luahighlighter.h"

#include <QtCore/QFile>

LuaHighlighter::LuaHighlighter(QTextDocument *parent) : StyleSyntaxHighlighter(parent)
{
    requirePattern = QRegularExpression(R"((require\s*([(\"'][a-zA-Z0-9*._]+['\")])))");
    functionPattern = QRegularExpression(R"((\b([A-Za-z0-9_]+(?:\s+|::))*([A-Za-z0-9_]+)(?= $)))");
    defTypePattern =
            QRegularExpression(R"((\b([A-Za-z0-9_]+)\s+([A-Za-z]{1}[A-Za-z0-9_]+)\s*[=]))");

    loadLanguageFile(":/config/lua.xml");
    initRules();
}

LuaHighlighter::~LuaHighlighter() { }

void LuaHighlighter::loadLanguageFile(const QString &fileName)
{
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return;

    Language language(&file);
    if (!language.isLoaded())
        return;

    const QStringList keys = language.getKeys();
    for (const QString &key : keys) {
        const QStringList names = language.getNames(key);
        for (const QString &name : names) {
            HighlightRule rule;
            rule.pattern.setPattern(QString(R"((\b\s{0,1}%1\s{0,1}\b))").arg(name));
            rule.format = key;
            highlightRules.append(rule);
        }
    }
}

void LuaHighlighter::initRules()
{
    highlightRules.append({ QRegularExpression(R"(\b(0b|0x)?[\d.']+\b)"), "Number" });
    highlightRules.append({ QRegularExpression(R"(["][^\n"]*["]|['][^\n']*['])"), "String" });
    highlightRules.append({ QRegularExpression(R"(#\![a-zA-Z_]+)"), "Preprocessor" });
    highlightRules.append({ QRegularExpression(R"(--[^\n]*)"), "Comment" });
    highlightBlockRules.append({QRegularExpression(R"(-- $$ $$))"), QRegularExpression(R"(--$$ $$ ))", "Comment"});
    highlightBlockRules.append(
            { QRegularExpression(R"( $$ $$))"), QRegularExpression(R"($$ $$ )"), "String" });
}

void LuaHighlighter::highlightBlock(const QString &text)
{
    auto matchIterator = requirePattern.globalMatch(text);
    while (matchIterator.hasNext()) {
        auto match = matchIterator.next();
        setFormat(match.capturedStart(), match.capturedLength(),
                  getSyntaxStyle()->getFormat("Preprocessor"));
        setFormat(match.capturedStart(2), match.capturedLength(2),
                  getSyntaxStyle()->getFormat("String"));
    }

    matchIterator = functionPattern.globalMatch(text);
    while (matchIterator.hasNext()) {
        auto match = matchIterator.next();
        setFormat(match.capturedStart(), match.capturedLength(),
                  getSyntaxStyle()->getFormat("Type"));
        setFormat(match.capturedStart(3), match.capturedLength(3),
                  getSyntaxStyle()->getFormat("Function"));
    }

    matchIterator = defTypePattern.globalMatch(text);
    while (matchIterator.hasNext()) {
        auto match = matchIterator.next();
        setFormat(match.capturedStart(3), match.capturedLength(3),
                  getSyntaxStyle()->getFormat("Type"));
    }

    for (const auto &rule : highlightRules) {
        matchIterator = rule.pattern.globalMatch(text);
        while (matchIterator.hasNext()) {
            auto match = matchIterator.next();
            setFormat(match.capturedStart(), match.capturedLength(),
                      getSyntaxStyle()->getFormat(rule.format));
        }
    }

    int state = previousBlockState();
    int startIndex = 0;

    if (state < 1 || state > highlightBlockRules.size()) {
        for (std::size_t i = 0; i < highlightBlockRules.size(); ++i) {
            auto startMatch = highlightBlockRules[i].start.match(text);
            startIndex = startMatch.capturedStart();
            if (startIndex >= 0) {
                state = i + 1;
                break;
            }
        }
    }

    int matchLen = 0;
    while (startIndex >= 0) {
        const auto &rule = highlightBlockRules[state - 1];
        auto endMatch = rule.end.match(text, startIndex);
        int endIndex = endMatch.capturedStart();
        if (endIndex == -1) {
            setCurrentBlockState(state);
            matchLen = text.length() - startIndex;
        } else {
            matchLen = endIndex - startIndex + endMatch.capturedLength();
        }
        setFormat(startIndex, matchLen, getSyntaxStyle()->getFormat(rule.format));
        auto startMatch = rule.start.match(text, startIndex + matchLen);
        startIndex = startMatch.capturedStart();
    }
}
