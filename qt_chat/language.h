#ifndef LANGUAGE_H
#define LANGUAGE_H

#include <QtCore/QObject>
#include <QtCore/QIODevice>
#include <QtCore/QXmlStreamReader>
#include <QtCore/QMap>
#include <QtCore/QStringList>

class Language : public QObject
{
    Q_OBJECT
public:
    explicit Language(QIODevice *device, QObject *parent = nullptr);
    ~Language();

    bool load(QIODevice *device);
    QStringList getKeys() const;
    QStringList getNames(const QString &key) const;
    bool isLoaded() const;

private:
    bool loaded;
    QMap<QString, QStringList> list;
};

#endif // LANGUAGE_H
