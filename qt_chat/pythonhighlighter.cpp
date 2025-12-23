#include "pythonhighlighter.h"

#include <QtCore/QFile>

PythonHighlighter::PythonHighlighter(QTextDocument *parent) : StyleSyntaxHighlighter(parent)
{
    includePattern = QRegularExpression(
            R"((?:from\s+(\w+(?:\.\w+)*)\s+)?import\s+((?:\w+(?:\s+as\s+\w+)?)(?:\s*,\s*\w+(?:\s+as\s+\w+)?)*|\*))");

    loadLanguageFile(":/config/python.xml");
    initRules();
}

PythonHighlighter::~PythonHighlighter() { }

void PythonHighlighter::loadLanguageFile(const QString &fileName)
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

void PythonHighlighter::initRules()
{
    highlightRules.append(
            { QRegularExpression(R"(\b(0b|0x)?(\d{1,3}(_?\d{3})*(\.\d+)?)\b)"), "Number" });

    highlightStringRules.append({ QRegularExpression(R"("[^\n"]*")"), "String" });
    highlightStringRules.append({ QRegularExpression(R"('[^\n']*')"), "String" });

    highlightRules.append({ QRegularExpression(R"(#[^\n]*)"), "Comment" });

    highlightBlockRules.append(
            { QRegularExpression(R"(""")"), QRegularExpression(R"(""")"), "String" });
    highlightBlockRules.append(
            { QRegularExpression(R"(''')"), QRegularExpression(R"(''')"), "String" });
}

void PythonHighlighter::highlightBlock(const QString &text)
{
    auto matchIterator = includePattern.globalMatch(text);
    while (matchIterator.hasNext()) {
        auto match = matchIterator.next();
        setFormat(match.capturedStart(), match.capturedLength(),
                  getSyntaxStyle()->getFormat("Module"));
    }

    for (const HighlightRule &rule : highlightRules) {
        auto matchIterator = rule.pattern.globalMatch(text);
        while (matchIterator.hasNext()) {
            auto match = matchIterator.next();
            setFormat(match.capturedStart(), match.capturedLength(),
                      getSyntaxStyle()->getFormat(rule.formatName));
        }
    }

    setCurrentBlockState(0);
    int startIndex = 0;
    int ruleId = previousBlockState();

    QVector<int> candidates;
    if (ruleId < 1 || ruleId > highlightBlockRules.size()) {
        for (int i = 0; i < highlightBlockRules.size(); ++i) {
            auto m = highlightBlockRules[i].begin.match(text);
            candidates.append(m.capturedStart());
        }
        int best = -1;
        for (int idx : candidates) {
            if (idx >= 0 && (best == -1 || idx < best))
                best = idx;
        }
        if (best != -1)
            ruleId = candidates.indexOf(best) + 1;
        startIndex = best;
    }

    HighlightBlockRule blockRule;
    QRegularExpressionMatch startMatch;
    if (startIndex >= 0) {
        blockRule = highlightBlockRules[ruleId - 1];
        if (previousBlockState() <= 0)
            startMatch = blockRule.begin.match(text);
    }

    if (!text.isEmpty() && startIndex != 0)
        singleLineStrHighlight(text, 0, startIndex);

    while (startIndex >= 0) {
        QRegularExpressionMatch endMatch;
        int startLen =
                (previousBlockState() > 0 && startIndex == 0) ? 0 : startMatch.capturedLength();
        endMatch = blockRule.end.match(text, startIndex + startLen);
        int endIndex = endMatch.capturedStart();
        int matchLen = 0;

        if (endIndex == -1) {
            setCurrentBlockState(ruleId);
            matchLen = text.length() - startIndex;
        } else {
            matchLen = endIndex - startIndex + endMatch.capturedLength();
        }
        setFormat(startIndex, matchLen, getSyntaxStyle()->getFormat(blockRule.formatName));

        candidates.clear();
        for (int i = 0; i < highlightBlockRules.size(); ++i) {
            auto m = highlightBlockRules[i].begin.match(text, startIndex + matchLen);
            candidates.append(m.capturedStart());
        }
        int best = -1;
        for (int idx : candidates) {
            if (idx >= 0 && (best == -1 || idx < best))
                best = idx;
        }
        if (best != -1) {
            ruleId = candidates.indexOf(best) + 1;
            startMatch = highlightBlockRules[ruleId - 1].begin.match(text, startIndex + matchLen);
        }
        int stringStart = startIndex + matchLen;
        startIndex = best;
        if (stringStart != text.length() && stringStart != startIndex)
            singleLineStrHighlight(text, stringStart, startIndex);
    }
}

void PythonHighlighter::singleLineStrHighlight(const QString &text, int start, int end)
{
    QStringRef slice = (end == -1) ? text.midRef(start) : text.midRef(start, end - start + 1);
    if (slice.isEmpty())
        return;

    struct Cand
    {
        int pos;
        int ruleIdx;
    };
    QVector<Cand> cands;

    for (int i = 0; i < highlightStringRules.size(); ++i) {
        auto m = highlightStringRules[i].pattern.match(slice.toString());
        if (m.hasMatch())
            cands.append({ m.capturedStart(), i });
    }
    if (cands.isEmpty())
        return;

    std::sort(cands.begin(), cands.end(),
              [](const Cand &a, const Cand &b) { return a.pos < b.pos; });

    int offset = 0;
    while (!cands.isEmpty()) {
        Cand best = cands.takeFirst();
        const auto &rule = highlightStringRules[best.ruleIdx];
        auto m = rule.pattern.match(slice.toString(), offset);
        if (!m.hasMatch())
            break;
        setFormat(start + m.capturedStart(), m.capturedLength(),
                  getSyntaxStyle()->getFormat(rule.formatName));
        offset = m.capturedStart() + m.capturedLength();

        cands.clear();
        for (int i = 0; i < highlightStringRules.size(); ++i) {
            auto mm = highlightStringRules[i].pattern.match(slice.toString(), offset);
            if (mm.hasMatch())
                cands.append({ mm.capturedStart(), i });
        }
        std::sort(cands.begin(), cands.end(),
                  [](const Cand &a, const Cand &b) { return a.pos < b.pos; });
    }
}
