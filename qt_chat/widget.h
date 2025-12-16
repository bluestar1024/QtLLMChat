#ifndef WIDGET_H
#define WIDGET_H

#pragma once
#include <QtWidgets/QWidget>
#include <QtGui/QMouseEvent>

class Widget : public QWidget
{
    Q_OBJECT
public:
    explicit Widget(QWidget *parent = nullptr);
    ~Widget();

protected:
    void mouseMoveEvent(QMouseEvent *e) override;
};

#endif // WIDGET_H
