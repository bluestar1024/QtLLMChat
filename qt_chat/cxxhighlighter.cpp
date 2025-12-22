#include "cxxhighlighter.h"

#include <QtCore/QFile>

QCXXHighlighter::QCXXHighlighter(QTextDocument *parent) : StyleSyntaxHighlighter(parent)
{
    includePattern.setPattern(R"(^\s*#\s*include\s*([<"][^:?"<>\|]+[">]))");
    commentStartPattern.setPattern(R"(/\*)");
    commentEndPattern.setPattern(R"(\*/)");

    loadLanguageFile(":/config/cpp.xml");
    initRules();
}

QCXXHighlighter::~QCXXHighlighter() { }

void QCXXHighlighter::loadLanguageFile(const QString &fileName)
{
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return;

    Language language(file);
    if (!language.isLoaded())
        return;

    QStringList keys = language.getKeys();
    for (const QString &key : keys) {
        QStringList names = language.getNames(key);
        for (const QString &name : names) {
            HighlightRule rule;
            rule.pattern.setPattern(QString(R"(\b%1\b)").arg(QRegularExpression::escape(name)));
            rule.format = key;
            highlightRules.append(rule);
        }
    }
}

void QCXXHighlighter::initRules()
{
    HighlightRule number;
    number.pattern.setPattern(
            R"((?<=\b|\s|^)(?i)(?:(?:(?:(?:(?:\d+(?:\'\d+)*)?\.(?:\d+(?:\'\d+)*)(?:e[+-]?(?:\d+(?:\'\d+)*))?)|(?:(?:\d+(?:\'\d+)*)\.(?:e[+-]?(?:\d+(?:\'\d+)*))?)|(?:(?:\d+(?:\'\d+)*)(?:e[+-]?(?:\d+(?:\'\d+)*)))|(?:0x(?:[0-9a-f]+(?:\'[0-9a-f]+)*)?\.(?:[0-9a-f]+(?:\'[0-9a-f]+)*)(?:p[+-]?(?:\d+(?:\'\d+)*)))|(?:0x(?:[0-9a-f]+(?:\'[0-9a-f]+)*)\.?(?:p[+-]?(?:\d+(?:\'\d+)*))))[lf]?)|(?:(?:(?:[1-9]\d*(?:\'\d+)*)|(?:0[0-7]*(?:\'[0-7]+)*)|(?:0x[0-9a-f]+(?:\'[0-9a-f]+)*)|(?:0b[01]+(?:\'[01]+)*))(?:u?l{0,2}|l{0,2}u?)))(?=\b|\s|$))");
    number.format = QStringLiteral("Number");
    highlightRules.append(number);

    HighlightRule str;
    str.pattern.setPattern(R"("[^\n"]*")");
    str.format = QStringLiteral("String");
    highlightRules.append(str);

    HighlightRule pre;
    pre.pattern.setPattern(R"(#[a-zA-Z_]+)");
    pre.format = QStringLiteral("Preprocessor");
    highlightRules.append(pre);

    HighlightRule single;
    single.pattern.setPattern(R"(//[^\n]*)");
    single.format = QStringLiteral("Comment");
    highlightRules.append(single);
}

void QCXXHighlighter::highlightBlock(const QString &text)
{
    auto matchIterator = includePattern.globalMatch(text);
    while (matchIterator.hasNext()) {
        auto match = matchIterator.next();
        setFormat(match.capturedStart(), match.capturedLength(),
                  syntaxStyle()->getFormat("Preprocessor"));
        setFormat(match.capturedStart(1), match.capturedLength(1),
                  syntaxStyle()->getFormat("String"));
    }

    for (const auto &rule : qAsConst(highlightRules)) {
        auto matchIterator = rule.pattern.globalMatch(text);
        while (matchIterator.hasNext()) {
            auto match = matchIterator.next();
            setFormat(match.capturedStart(), match.capturedLength(),
                      syntaxStyle()->getFormat(rule.format));
        }
    }

    // 3. 多行注释 /* ... */
    setCurrentBlockState(0);
    int startIndex = 0;
    if (previousBlockState() != 1) {
        auto startMatch = commentStartPattern.match(text);
        startIndex = startMatch.capturedStart();
    }

    while (startIndex >= 0) {
        auto endMatch = commentEndPattern.match(text, startIndex);
        int commentLen = 0;
        if (!endMatch.hasMatch()) {
            setCurrentBlockState(1);
            commentLen = text.length() - startIndex;
        } else {
            commentLen = endMatch.capturedEnd() - startIndex + 1;
        }
        setFormat(startIndex, commentLen, syntaxStyle()->getFormat("Comment"));

        auto nextMatch = commentStartPattern.match(text, startIndex + commentLen);
        startIndex = nextMatch.capturedStart();
    }
}
