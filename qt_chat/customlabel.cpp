#include "customlabel.h"

CustomLabel::CustomLabel(QWidget *parent) : QLabel(parent) { }

CustomLabel::~CustomLabel() { }

void CustomLabel::mousePressEvent(QMouseEvent *e)
{
    QLabel::mousePressEvent(e);
    e->ignore();
}

void CustomLabel::mouseReleaseEvent(QMouseEvent *e)
{
    QLabel::mouseReleaseEvent(e);
    e->ignore();
}

void CustomLabel::contextMenuEvent(QContextMenuEvent *e)
{
    e->ignore();
}
