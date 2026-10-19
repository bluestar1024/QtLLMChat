#include "listwidget.h"

ListWidget::ListWidget(QWidget *parent)
    : QListWidget(parent), scrollAutoChange(true), scrollChangeUplimit(200), pendingSendItem(false)
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

void ListWidget::scrollTo(const QModelIndex &, ScrollHint) { }

void ListWidget::resetScrollAutoChange()
{
    scrollAutoChange = true;
    // 上次量测异常导致挂起残留时兜底恢复（正常路径由 MessageWidget::resizeFinished 解除）
    pendingSendItem = false;
}

void ListWidget::setPendingSendItem(bool pending)
{
    pendingSendItem = pending;
}

/* 滚动条范围变化时的槽 */
void ListWidget::onScrollBarRangeChanged(int min, int max)
{
    Q_UNUSED(min);
    // 新增消息量测未完成（临时高度）期间不滚动：此时 max 是"假底部"，
    // 滚过去只会看到未显示的空白 item 区；量测完成后 rangeChanged 会再次触发
    if (scrollAutoChange && !pendingSendItem)
        verticalScrollBar()->setValue(max);
}

/* 滚动条数值变化时的槽 */
void ListWidget::onScrollBarValueChanged(int value)
{
    Q_UNUSED(value);
    if (verticalScrollBar()->maximum() - verticalScrollBar()->value() >= scrollChangeUplimit)
        scrollAutoChange = false;
    else
        scrollAutoChange = true;
}

void ListWidget::mouseMoveEvent(QMouseEvent *e)
{
    QListWidget::mouseMoveEvent(e);
    e->ignore();
}

void ListWidget::mousePressEvent(QMouseEvent *e)
{
    QListWidget::mousePressEvent(e);
    e->ignore();
}

void ListWidget::mouseReleaseEvent(QMouseEvent *e)
{
    QListWidget::mouseReleaseEvent(e);
    e->ignore();
}
