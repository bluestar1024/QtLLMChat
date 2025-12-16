#include "widget.h"

Widget::Widget(QWidget *parent) : QWidget(parent)
{
    setMouseTracking(true);
}

Widget::~Widget() { }

void Widget::mouseMoveEvent(QMouseEvent *e)
{
    QWidget::mouseMoveEvent(e);
    e->ignore();
}
