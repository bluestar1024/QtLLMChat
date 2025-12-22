#include "syntaxstyle.h"

#include <QtGui/QFont>

SyntaxStyle::SyntaxStyle(QObject *parent) : QObject{ parent }, name(""), loaded(false) { }

SyntaxStyle::~SyntaxStyle() { }

bool SyntaxStyle::load(const QString &xmlContent)
{
    QXmlStreamReader reader(xmlContent);

    while (!reader.atEnd() && !reader.hasError()) {
        auto token = reader.readNext();

        if (token == QXmlStreamReader::StartElement) {
            if (reader.name() == "style-scheme") {
                if (reader.attributes().hasAttribute("name"))
                    name = reader.attributes().value("name").toString();
            } else if (reader.name() == "style") {
                auto attrs = reader.attributes();
                QString name = attrs.value("name").toString();
                QTextCharFormat fmt;

                if (attrs.hasAttribute("background"))
                    fmt.setBackground(QColor(attrs.value("background").toString()));

                if (attrs.hasAttribute("foreground"))
                    fmt.setForeground(QColor(attrs.value("foreground").toString()));

                if (attrs.hasAttribute("bold") && attrs.value("bold") == "true")
                    fmt.setFontWeight(QFont::Bold);

                if (attrs.hasAttribute("italic") && attrs.value("italic") == "true")
                    fmt.setFontItalic(true);

                if (attrs.hasAttribute("underlineStyle")) {
                    QString ul = attrs.value("underlineStyle").toString();
                    QTextCharFormat::UnderlineStyle s = QTextCharFormat::NoUnderline;

                    if (ul == "SingleUnderline")
                        s = QTextCharFormat::SingleUnderline;
                    else if (ul == "DashUnderline")
                        s = QTextCharFormat::DashUnderline;
                    else if (ul == "DotLine")
                        s = QTextCharFormat::DotLine;
                    else if (ul == "DashDotLine")
                        s = QTextCharFormat::DashDotLine;
                    else if (ul == "DashDotDotLine")
                        s = QTextCharFormat::DashDotDotLine;
                    else if (ul == "WaveUnderline")
                        s = QTextCharFormat::WaveUnderline;
                    else if (ul == "SpellCheckUnderline")
                        s = QTextCharFormat::SpellCheckUnderline;
                    else
                        qWarning() << "Unknown underline value" << ul;

                    fmt.setUnderlineStyle(s);
                }
                data.insert(name, fmt);
            }
        }
    }

    loaded = !reader.hasError();
    return loaded;
}

QString SyntaxStyle::getName()
{
    return name;
}

QTextCharFormat SyntaxStyle::getFormat(const QString &name) const
{
    return data.value(name, QTextCharFormat());
}

bool SyntaxStyle::isLoaded()
{
    return loaded;
}

SyntaxStyle *SyntaxStyle::defaultStyle()
{
    static SyntaxStyle style;
    if (!style.isLoaded()) {
        QFile f(codeThemeFilePath);
        if (!f.open(QIODevice::ReadOnly | QIODevice::Text)) {
            qWarning() << "Can't open default style file.";
            return &style;
        }
        QString content = QString::fromUtf8(f.readAll());
        if (!style.load(content))
            qWarning() << "Can't load default style.";
    }
    return &style;
}
