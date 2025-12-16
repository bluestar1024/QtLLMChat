#ifndef TEXTEDIT_H
#define TEXTEDIT_H

#pragma once
#include "sendbutton.h"
#include "custommenu.h"

#include <QtWidgets/QTextEdit>
#include <QtWidgets/QSizePolicy>
#include <QtGui/QFontDatabase>
#include <QtGui/QFont>
#include <QtWidgets/QGridLayout>
#include <QtGui/QContextMenuEvent>
#include <QtWidgets/QMenu>
#include <QtGui/QAction>
#include <QtGui/QKeySequence>
#include <QtGui/QIcon>
#include <QtGui/QMouseEvent>
#include <QtGui/QKeyEvent>
#include <QtCore/QMetaMethod>

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
    void contextMenuEvent(QContextMenuEvent *e) override;
    void mouseMoveEvent(QMouseEvent *e) override;
    void mousePressEvent(QMouseEvent *e) override;
    void mouseReleaseEvent(QMouseEvent *e) override;
    void keyPressEvent(QKeyEvent *e) override;

private slots:
    void sendButtonShow();

private:
    SendButton *sendButton;
    QGridLayout *gLayout;
    QString sendImagesPath;
    QString sendHoverImagesPath;
    QString sendDisableImagesPath;
    bool isSending;
    QMap<QString, QVariant> widgetSizeDict;
};

template <typename T>
void TextEdit::connectSendButtonClick(T *receiver, void (T::*slot)())
{
    connect(sendButton, &QPushButton::clicked, [=](bool) { (receiver->*slot)(); });
}

template <typename T>
void TextEdit::connectSendButtonClick(T *receiver, void (T::*slot)(bool))
{
    connect(sendButton, &QPushButton::clicked, receiver, slot);
}

#endif // TEXTEDIT_H
