#ifndef TEXTWIDGET_H
#define TEXTWIDGET_H

#pragma once
#include <QtWidgets/QWidget>
#include <QtGui/QPainter>
#include <QtGui/QPainterPath>
#include <QtCore/QRect>
#include <QtGui/QBrush>

class TextWidget : public QWidget
{
    Q_OBJECT

public:
    explicit TextWidget(bool isUser = true, QWidget *parent = nullptr);
    ~TextWidget();

protected:
    void paintEvent(QPaintEvent *) override;

private:
    bool isUser;
};

#endif // TEXTWIDGET_H
