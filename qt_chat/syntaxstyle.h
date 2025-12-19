#ifndef SYNTAXSTYLE_H
#define SYNTAXSTYLE_H

#pragma once
#include <QObject>
#include <QXmlStreamReader>
#include <QTextCharFormat>
#include <QHash>
#include <QFile>
#include <QIODevice>

extern const QString codeThemeFilePath;

class SyntaxStyle : public QObject
{
    Q_OBJECT
public:
    explicit SyntaxStyle(QObject *parent = nullptr);

    bool load(const QString &xmlContent);
    QString name() const { return name; }
    bool isLoaded() const { return loaded; }
    QTextCharFormat getFormat(const QString &name) const;
    static SyntaxStyle *defaultStyle();

private:
    QString name;
    QHash<QString, QTextCharFormat> data;
    bool loaded = false;
};

#endif // SYNTAXSTYLE_H
