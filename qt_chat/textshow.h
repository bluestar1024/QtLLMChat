#ifndef TEXTSHOW_H
#define TEXTSHOW_H

#pragma once
#include <QWidget>
#include <QFont>
#include <QFontDatabase>
#include <QFontMetricsF>
#include <QHBoxLayout>
#include <QTimer>
#include <QUrl>
#include <QFileInfo>
#include "customlabel.h"
#include "webengineview.h"
#include "markdown_parser.h"
#include "html_renderer.h"

extern const QString fontFilePath;
extern const QString mathjaxScriptPath;
extern const int windowFontPixelSize;

class WebEngineView;

class TextShow : public QWidget
{
    Q_OBJECT
public:
    explicit TextShow(const QString &text = "", bool isUser = true, int maxWidth = 810,
                      QWidget *parent = nullptr);
    ~TextShow();

    template <typename T>
    void connectExecuteNext(T *receiver, void (T::*slot)());

    void setText(const QString &text);
    void toggleWidget();
    bool hasSelectedText() const;
    QString selectedText() const;

signals:
    void setSizeFinished();
    void executeNext();

private slots:
    void onPageLoadFinished(bool success);
    void onContentsSizeChanged(const QSizeF &size);
    void onUpdateSize();

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

    QString m_text;
    bool m_isUser;
    int m_maxWidth;
    bool m_isLabel = true;
    bool m_firstExecuteNextEmit = true;

    CustomLabel *m_label;
    WebEngineView *m_webEngineView;
    QHBoxLayout *m_mainHLayout;
    QFont m_font;
    QFontMetricsF *m_fontMetrics = nullptr;
    QTimer *m_updateSizeTimer;

    QString m_mathJaxCdn;
    QString m_htmlText;
    QString m_fullHtmlText;
};

template <typename T>
void TextShow::connectExecuteNext(T *receiver, void (T::*slot)())
{
    connect(this, &TextShow::executeNext, receiver, slot);
}

#endif // TEXTSHOW_H
