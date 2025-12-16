#ifndef TEXTEDITFULL_H
#define TEXTEDITFULL_H

#pragma once
#include "textedit.h"

#include <QtWidgets/QWidget>
#include <QtWidgets/QHBoxLayout>
#include <QtCore/QPropertyAnimation>
#include <QtGui/QMouseEvent>
#include <QtGui/QPaintEvent>
#include <QtGui/QPainter>
#include <QtGui/QPen>
#include <QtGui/QBrush>
#include <QtGui/QPainterPath>
#include <QtGui/QColor>
#include <QtCore/QMetaMethod>

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
    void mouseMoveEvent(QMouseEvent *e) override;
    void paintEvent(QPaintEvent *e) override;

private:
    TextEdit *textEdit;
    QHBoxLayout *mainHLayout;
    QPropertyAnimation *animationBackgroundColor;
    QPropertyAnimation *animationBorderColor;
    QColor bgColor;
    QColor bColor;
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
