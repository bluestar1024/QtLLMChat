#include "splitter.h"

Splitter::Splitter(Qt::Orientation orientation, QWidget *parent) : QSplitter(orientation, parent)
{
    setMouseTracking(true);
}

Splitter::~Splitter() { }

void Splitter::mouseMoveEvent(QMouseEvent *e)
{
    QSplitter::mouseMoveEvent(e);
    e->ignore();
}
