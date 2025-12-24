#include "glslhighlighter.h"

#include <QtCore/QFile>

GLSLHighlighter::GLSLHighlighter(QTextDocument *parent) : StyleSyntaxHighlighter(parent)
{
    includePattern = QRegularExpression(R"((#include\s+([<"][a-zA-Z0-9*._]+[">])))");
    functionPattern = QRegularExpression(R"((\b([A-Za-z0-9_]+(?:\s+|::))*([A-Za-z0-9_]+)(?= $))))");
    commentStartPattern = QRegularExpression(R"((/\*))");
    commentEndPattern = QRegularExpression(R"((\*/))");

    loadLanguageFile(":/config/glsl.xml");
    initRules();
}

GLSLHighlighter::~GLSLHighlighter() { }

void GLSLHighlighter::loadLanguageFile(const QString &fileName)
{
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly))
        return;

    Language language(&file);
    if (!language.isLoaded())
        return;

    const QStringList keys = language.getKeys();
    for (const QString &key : keys) {
        const QStringList names = language.getNames(key);
        for (const QString &name : names) {
            HighlightRule rule;
            rule.pattern.setPattern(QString(R"(\b%1\b)").arg(name));
            rule.format = key;
            highlightRules.append(rule);
        }
    }
}

void GLSLHighlighter::initRules()
{
    highlightRules.append({ QRegularExpression(R"(\b(0b|0x)?[\d']+\b)"), "Number" });
    highlightRules.append({ QRegularExpression(R"(#[a-zA-Z_]+)"), "Preprocessor" });
    highlightRules.append({ QRegularExpression(R"(//[^\n]*)"), "Comment" });
}

void GLSLHighlighter::highlightBlock(const QString &text)
{
    auto matchIterator = includePattern.globalMatch(text);
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

    for (const HighlightRule &rule : qAsConst(highlightRules)) {
        matchIterator = rule.pattern.globalMatch(text);
        while (matchIterator.hasNext()) {
            auto match = matchIterator.next();
            setFormat(match.capturedStart(), match.capturedLength(),
                      getSyntaxStyle()->getFormat(rule.format));
        }
    }

    setCurrentBlockState(0);
    int startIndex = 0;
    if (previousBlockState() != 1)
        startIndex = text.indexOf(commentStartPattern);

    QRegularExpressionMatch endMatch;
    int endIndex;
    int commentLen = 0;
    while (startIndex >= 0) {
        endIndex = text.indexOf(commentEndPattern, startIndex, &endMatch);
        if (endIndex == -1) {
            setCurrentBlockState(1);
            commentLen = text.length() - startIndex;
        } else {
            commentLen = endIndex - startIndex + endMatch.capturedLength();
        }
        setFormat(startIndex, commentLen, getSyntaxStyle()->getFormat("Comment"));
        startIndex = text.indexOf(commentStartPattern, startIndex + commentLen);
    }
}
