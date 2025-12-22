#ifndef SYNTAXSTYLE_H
#define SYNTAXSTYLE_H

#pragma once
#include <QtCore/QObject>
#include <QtCore/QXmlStreamReader>
#include <QtGui/QTextCharFormat>
#include <QtCore/QHash>
#include <QtCore/QFile>
#include <QtCore/QIODevice>

extern const QString codeThemeFilePath;

class SyntaxStyle : public QObject
{
    Q_OBJECT
public:
    explicit SyntaxStyle(QObject *parent = nullptr);
    ~SyntaxStyle();

    bool load(const QString &xmlContent);
    QString getName();
    QTextCharFormat getFormat(const QString &name) const;
    bool isLoaded();
    static SyntaxStyle *defaultStyle();

private:
    QString name;
    QHash<QString, QTextCharFormat> data;
    bool loaded;
};

#endif // SYNTAXSTYLE_H
