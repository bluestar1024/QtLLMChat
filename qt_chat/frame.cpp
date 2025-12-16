#include "frame.h"

Frame::Frame(QWidget *parent) : QFrame(parent)
{
    setMouseTracking(true);
}

Frame::~Frame() { }

void Frame::mouseMoveEvent(QMouseEvent *event)
{
    QFrame::mouseMoveEvent(event);
    event->ignore();
}
