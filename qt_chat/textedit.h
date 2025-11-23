#ifndef TEXTEDIT_H
#define TEXTEDIT_H

#pragma once
#include <QTextEdit>
#include <QSizePolicy>
#include <QFontDatabase>
#include <QFont>
#include <QGridLayout>
#include <QContextMenuEvent>
#include <QMenu>
#include <QAction>
#include <QKeySequence>
#include <QIcon>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QMetaMethod>
#include "sendbutton.h"
#include "custommenu.h"

extern const QString imagesDir;
extern const QString fontFilePath;
extern const int windowFontPointSize;
extern const int windowFontPixelSize;

class TextEdit : public QTextEdit
{
    Q_OBJECT

public:
    explicit TextEdit(QWidget *parent = nullptr);
    ~TextEdit();

    void updateSendButtonSize(qreal curDpi, qreal lastDpi);
    template <typename T>
    void connectSendButtonClick(T *receiver, void (T::*slot)());
    template <typename T>
    void connectSendButtonClick(T *receiver, void (T::*slot)(bool));
    void emitSendButtonClicked();
    void setSending(bool isSending);

protected:
    void contextMenuEvent(QContextMenuEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;

private slots:
    void sendButtonShow();

private:
    SendButton *sendButton;
    QGridLayout *gLayout;
    QString send_images_path;
    QString send_hover_images_path;
    QString send_disable_images_path;
    bool isSending;
    QMap<QString, QVariant> widgetSizeDict;
};

template <typename T>
void TextEdit::connectSendButtonClick(T *receiver, void (T::*slot)())
{
    connect(sendButton, &QPushButton::clicked, [=](bool) {
        (receiver->*slot)();
    });
}

template <typename T>
void TextEdit::connectSendButtonClick(T *receiver, void (T::*slot)(bool))
{
    connect(sendButton, &QPushButton::clicked, receiver, slot);
}

#endif // TEXTEDIT_H
