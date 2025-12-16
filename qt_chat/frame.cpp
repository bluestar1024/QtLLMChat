#include "frame.h"

Frame::Frame(QWidget *parent) : QFrame(parent)
{
    setMouseTracking(true);
}

Frame::~Frame() { }

void Frame::mouseMoveEvent(QMouseEvent *e)
{
    QFrame::mouseMoveEvent(e);
    e->ignore();
}
