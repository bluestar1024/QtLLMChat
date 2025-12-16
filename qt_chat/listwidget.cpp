#include "listwidget.h"

ListWidget::ListWidget(QWidget *parent)
    : QListWidget(parent), m_scrollAutoChange(true), m_scrollChangeUplimit(200)
{
    resize(1170, 480);
    setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
    setFocusPolicy(Qt::NoFocus);
    verticalScrollBar()->setCursor(Qt::PointingHandCursor);
    setStyleSheet("QListWidget{"
                  "    border: none;"
                  "    background: transparent;"
                  "}"
                  "QListWidget::item{"
                  "    background: transparent;"
                  "}"
                  "QListWidget::item:active{"
                  "    background: transparent;"
                  "}"
                  "QListWidget::item:selected{"
                  "    background: transparent;"
                  "}"
                  "QListWidget::item:hover{"
                  "    background: transparent;"
                  "}"
                  "QScrollBar:vertical {"
                  "    background: transparent;"
                  "    width: 10px;"
                  "    padding: 0px 4px 0px 0px;"
                  "}"
                  "QScrollBar::handle:vertical {"
                  "    background: #bcbcbc;"
                  "    width: 6px;"
                  "    border-radius: 3px;"
                  "}"
                  "QScrollBar::handle:vertical:hover {"
                  "    background: #808080;"
                  "}"
                  "QScrollBar::add-page:vertical, QScrollBar::sub-page:vertical {"
                  "    background: transparent;"
                  "}"
                  "QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical {"
                  "    width: 0px;"
                  "}");

    setMouseTracking(true);
    connect(verticalScrollBar(), &QScrollBar::rangeChanged, this,
            &ListWidget::onScrollBarRangeChanged);
    connect(verticalScrollBar(), &QScrollBar::valueChanged, this,
            &ListWidget::onScrollBarValueChanged);
}

ListWidget::~ListWidget() { }

void ListWidget::scrollTo(const QModelIndex &index, ScrollHint hint)
{
    Q_UNUSED(index);
    Q_UNUSED(hint);
}

/* 滚动条范围变化时的槽 */
void ListWidget::onScrollBarRangeChanged(int min, int max)
{
    Q_UNUSED(min);
    if (m_scrollAutoChange)
        verticalScrollBar()->setValue(max);
}

/* 滚动条数值变化时的槽 */
void ListWidget::onScrollBarValueChanged(int value)
{
    Q_UNUSED(value);
    if (verticalScrollBar()->maximum() - verticalScrollBar()->value() >= m_scrollChangeUplimit)
        m_scrollAutoChange = false;
    else
        m_scrollAutoChange = true;
}

void ListWidget::mouseMoveEvent(QMouseEvent *event)
{
    QListWidget::mouseMoveEvent(event);
    event->ignore();
}

void ListWidget::mousePressEvent(QMouseEvent *event)
{
    QListWidget::mousePressEvent(event);
    event->ignore();
}

void ListWidget::mouseReleaseEvent(QMouseEvent *event)
{
    QListWidget::mouseReleaseEvent(event);
    event->ignore();
}
