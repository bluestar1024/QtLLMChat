#ifndef CODESHOW_H
#define CODESHOW_H

#pragma once
#include "codeeditor.h"

#include <QtWidgets/QWidget>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QTextEdit>
#include <QtGui/QClipboard>
#include <QtGui/QFontDatabase>
#include <QtGui/QPainter>
#include <QtWidgets/QApplication>

extern const QString imagesDir;
extern const QString fontFilePath;
extern const int windowFontPointSize;

class CodeShow : public QWidget
{
    Q_OBJECT

public:
    explicit CodeShow(const QString &codeText, const QString &lexerName = "cpp", int maxWidth = 810,
                      QWidget *parent = nullptr);
    ~CodeShow();

    template <typename T>
    void connectCodeCopyButtonClick(T *receiver, void (T::*slot)());
    template <typename T>
    void connectCodeCopyButtonClick(T *receiver, void (T::*slot)(bool));
    QString getText() const;
    void setText(const QString &codeText, const QString &lexerName = "cpp");
    QString getLexerName() const;
    bool hasSelectedText() const;
    QString getSelectedText() const;

public slots:
    void toggleThemeStyle();
    void setLineWordWrapMode();
    void copyCode();

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    void setupUI();
    void applyDarkStyle();
    void applyLightStyle();

    QString codeText;
    QString lexerName;
    int maxWidth;

    QWidget *topWidget;
    QWidget *topSubLeftWidget;
    QWidget *topSubRightWidget;
    QLabel *label;
    PushButton *toggleThemeButton;
    PushButton *wordWrapButton;
    PushButton *codeCopyButton;
    CodeEditor *codeEdit;

    bool isLightThemeStyle;
    bool isWordWrap;

    QString lightThemeImagesPath;
    QString darkThemeImagesPath;
    QString lightWordWrapImagesPath;
    QString lightSingleLineImagesPath;
    QString darkWordWrapImagesPath;
    QString darkSingleLineImagesPath;
    QString lightCodeCopyImagesPath;
    QString darkCodeCopyImagesPath;

    QColor borderColor;
};

template <typename T>
void CodeShow::connectCodeCopyButtonClick(T *receiver, void (T::*slot)())
{
    connect(codeCopyButton, &QPushButton::clicked, [=](bool) { (receiver->*slot)(); });
}

template <typename T>
void CodeShow::connectCodeCopyButtonClick(T *receiver, void (T::*slot)(bool))
{
    connect(codeCopyButton, &QPushButton::clicked, receiver, slot);
}

#endif // CODESHOW_H
