#ifndef TEXTEDITFULL_H
#define TEXTEDITFULL_H

#pragma once
#include <QWidget>
#include <QHBoxLayout>
#include <QPropertyAnimation>
#include <QMouseEvent>
#include <QPaintEvent>
#include <QPainter>
#include <QPen>
#include <QBrush>
#include <QPainterPath>
#include <QColor>
#include <QMetaMethod>
#include "textedit.h"

extern const QColor textEditFullBGColor;
extern const QColor textEditFullBGTColor;
extern const QColor textEditFullBTColor;
extern const QColor textEditFullBColor;

class TextEditFull : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(QColor backgroundColor READ getBackgroundColor WRITE setBackgroundColor)
    Q_PROPERTY(QColor borderColor READ getBorderColor WRITE setBorderColor)

public:
    explicit TextEditFull(QWidget *parent = nullptr);
    ~TextEditFull();

    void backgroundColorShowLight();
    void backgroundColorShowDark();
    void clearFocus();
    void updateSendButtonSize(qreal curDpi, qreal lastDpi);
    void resetWidgetSize();
    QString toPlainText() const;
    void clearText();
    template <typename T>
    void connectSendButtonClick(T *receiver, void (T::*slot)());
    template <typename T>
    void connectSendButtonClick(T *receiver, void (T::*slot)(bool));
    void setSending(bool isSending);

protected:
    void mouseMoveEvent(QMouseEvent *event) override;
    void paintEvent(QPaintEvent *event) override;

private:
    TextEdit *textEdit;
    QHBoxLayout *mainHLayout;
    QPropertyAnimation *AnimationBackgroundColor;
    QPropertyAnimation *AnimationBorderColor;
    QColor BGColor;
    QColor BColor;
    bool backgroundColorIsLight;

    QColor getBackgroundColor() const;
    void setBackgroundColor(const QColor &color);
    QColor getBorderColor() const;
    void setBorderColor(const QColor &color);
};

template <typename T>
void TextEditFull::connectSendButtonClick(T *receiver, void (T::*slot)())
{
    textEdit->connectSendButtonClick(receiver, slot);
}

template <typename T>
void TextEditFull::connectSendButtonClick(T *receiver, void (T::*slot)(bool))
{
    textEdit->connectSendButtonClick(receiver, slot);
}

#endif // TEXTEDITFULL_H
