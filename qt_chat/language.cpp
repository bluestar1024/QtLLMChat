#include "language.h"

Language::Language(QIODevice *device, QObject *parent) : QObject(parent), loaded(false)
{
    load(device);
}

Language::~Language() { }

bool Language::load(QIODevice *device)
{
    if (!device)
        return false;

    QXmlStreamReader reader(device);
    QString currentName = "";
    QStringList stringList;
    bool readText = false;

    while (!reader.atEnd() && !reader.hasError()) {
        QXmlStreamReader::TokenType token = reader.readNext();

        if (token == QXmlStreamReader::StartElement) {
            QStringRef nameRef = reader.name();

            if (nameRef == QLatin1String("section")) {
                if (!stringList.isEmpty()) {
                    list[currentName] = stringList;
                    stringList.clear();
                }
                currentName = reader.attributes().value(QLatin1String("name")).toString();
            } else if (nameRef == QLatin1String("name")) {
                readText = true;
            }
        } else if (token == QXmlStreamReader::Characters && readText) {
            stringList.append(reader.text().toString());
            readText = false;
        }
    }

    if (!stringList.isEmpty())
        list[currentName] = stringList;

    loaded = !reader.hasError();
    return loaded;
}

QStringList Language::getKeys() const
{
    return list.keys();
}

QStringList Language::getNames(const QString &key) const
{
    return list.value(key);
}

bool Language::isLoaded() const
{
    return loaded;
}
