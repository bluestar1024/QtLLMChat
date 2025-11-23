#ifndef WIDGET_H
#define WIDGET_H

#pragma once
#include <QWidget>
#include <QMouseEvent>

class Widget : public QWidget
{
    Q_OBJECT
public:
    explicit Widget(QWidget *parent = nullptr);
    ~Widget();

protected:
    void mouseMoveEvent(QMouseEvent *event) override;
};

#endif // WIDGET_H
