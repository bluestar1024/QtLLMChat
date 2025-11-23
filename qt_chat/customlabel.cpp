#include "customlabel.h"

CustomLabel::CustomLabel(QWidget *parent)
    : QLabel(parent)
{
}

CustomLabel::~CustomLabel()
{
}

void CustomLabel::mousePressEvent(QMouseEvent *event)
{
    QLabel::mousePressEvent(event);
    event->ignore();
}

void CustomLabel::mouseReleaseEvent(QMouseEvent *event)
{
    QLabel::mouseReleaseEvent(event);
    event->ignore();
}

void CustomLabel::contextMenuEvent(QContextMenuEvent *event)
{
    event->ignore();
}
