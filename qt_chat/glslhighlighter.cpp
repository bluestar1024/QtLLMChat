#include "glslhighlighter.h"

#include <QFile>

GLSLHighlighter::GLSLHighlighter(QTextDocument *parent) : StyleSyntaxHighlighter(parent)
{
    includePattern = QRegularExpression(R"(#include\s+([<"][a-zA-Z0-9*._]+[">]))");
    functionPattern = QRegularExpression(R"((\b([A-Za-z0-9_]+(?:\s+|::))*([A-Za-z0-9_]+)(?= $))))");
    defTypePattern = QRegularExpression(R"((\b([A-Za-z0-9_]+)\s+[A-Za-z]{1}[A-Za-z0-9_]+\s*[;=]))");
    commentStartPattern = QRegularExpression(R"(/\*)");
    commentEndPattern = QRegularExpression(R"(\*/)");

    loadLanguageFile(":/config/glsl.xml");
    initRules();
}

void GLSLHighlighter::loadLanguageFile(const QString &fileName)
{
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly))
        return;

    Language language(&file);
    if (!language.isLoaded())
        return;

    const QStringList keys = language.keys();
    for (const QString &key : keys) {
        const QStringList names = language.names(key);
        for (const QString &name : names) {
            QRegularExpression re(QString(R"(\b%1\b)").arg(name));
            m_highlightRules.append({ re, key });
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
    auto it = includePattern.globalMatch(text);
    while (it.hasNext()) {
        auto match = it.next();
        setFormat(match.capturedStart(), match.capturedLength(),
                  syntaxStyle()->getFormat("Preprocessor"));
        setFormat(match.capturedStart(1), match.capturedLength(1),
                  syntaxStyle()->getFormat("String"));
    }

    it = functionPattern.globalMatch(text);
    while (it.hasNext()) {
        auto match = it.next();
        setFormat(match.capturedStart(), match.capturedLength(), syntaxStyle()->getFormat("Type"));
        setFormat(match.capturedStart(3), match.capturedLength(3),
                  syntaxStyle()->getFormat("Function"));
    }

    for (const HighlightRule &rule : qAsConst(highlightRules)) {
        it = rule.first.globalMatch(text);
        while (it.hasNext()) {
            auto match = it.next();
            setFormat(match.capturedStart(), match.capturedLength(),
                      syntaxStyle()->getFormat(rule.second));
        }
    }

    setCurrentBlockState(0);
    int startIndex = 0;
    if (previousBlockState() != 1)
        startIndex = text.indexOf(commentStartPattern);

    while (startIndex >= 0) {
        QRegularExpressionMatch endMatch;
        int endIndex = text.indexOf(commentEndPattern, startIndex, &endMatch);
        int commentLen;
        if (endIndex == -1) {
            setCurrentBlockState(1);
            commentLen = text.length() - startIndex;
        } else {
            commentLen = endIndex - startIndex + endMatch.capturedLength();
        }
        setFormat(startIndex, commentLen, syntaxStyle()->getFormat("Comment"));

        startIndex = text.indexOf(commentStartPattern, startIndex + commentLen);
    }
}
