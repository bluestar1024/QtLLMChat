#include "splitter.h"

#include <QtWidgets/QSplitterHandle>

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

QSplitterHandle *Splitter::createHandle()
{
    QSplitterHandle *handle = QSplitter::createHandle();
    Q_ASSERT(handle);
    handle->setCursor(Qt::ArrowCursor);
    handle->setAttribute(Qt::WA_TransparentForMouseEvents);
    return handle;
}
