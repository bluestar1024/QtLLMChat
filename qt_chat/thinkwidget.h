#ifndef THINKWIDGET_H
#define THINKWIDGET_H

#pragma once
#include "customlabel.h"
#include "webengineview.h"
#include "markdown_parser.h"
#include "html_renderer.h"

#include <QtWidgets/QWidget>
#include <QtGui/QFont>
#include <QtGui/QFontDatabase>
#include <QtGui/QFontMetricsF>
#include <QtWidgets/QHBoxLayout>
#include <QtCore/QTimer>
#include <QtCore/QUrl>
#include <QtCore/QFileInfo>

extern const QString fontFilePath;
extern const QString mathjaxScriptPath;
extern const int windowFontPixelSize;

class WebEngineView;

class ThinkWidget : public QWidget
{
    Q_OBJECT
public:
    explicit ThinkWidget(const QString &text = "", int maxWidth = 810, QWidget *parent = nullptr);
    ~ThinkWidget();

    void setText(const QString &text);
    void toggleWidget();
    bool hasSelectedText() const;
    QString selectedText() const;

    WebEngineView *webEngineView;
    QTimer *updateSizeTimer;

signals:
    void setSizeFinished();

private slots:
    void onPageLoadFinished(bool success);
    void onContentsSizeChanged(const QSizeF &size);
    virtual void onUpdateSize();

private:
    void measureText(const QString &text, int &labelWidth, int &labelHeight) const;
    QString htmlReplaceText(const QString &text) const;
    struct TableInfo
    {
        QString tableText;
        QStringList items;
        QStringList alignList;
        int row = 0;
        int col = 0;
        bool complete = false;
    };
    TableInfo getTable(const QString &text) const;
    QString getAlignmentClass(const QString &fmt) const;

    QString text;
    int maxWidth;
    bool isLabel;

    CustomLabel *label;
    QHBoxLayout *mainHLayout;
    QFont font;
    QFontMetricsF *fontMetrics;

    QString mathJaxCdn;
    QString htmlText;
    QString fullHtmlText;
};

#endif // THINKWIDGET_H
