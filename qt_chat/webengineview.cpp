#include "webengineview.h"
#include "messagewidget.h"

WebEngineView::WebEngineView(QWidget *parent) : QWebEngineView(parent)
{
    setPage(new WebEnginePage(this));
    page()->setBackgroundColor(Qt::transparent);
    load(QUrl());
    focusProxy()->installEventFilter(this);
}

WebEngineView::~WebEngineView() { }

bool WebEngineView::eventFilter(QObject *o, QEvent *e)
{
    if (o == focusProxy() && e->type() == QEvent::MouseButtonRelease) {
        auto *me = static_cast<QMouseEvent *>(e);
        QMouseEvent newMe(me->type(), me->position(), me->globalPosition(), me->button(),
                          me->buttons(), me->modifiers());
        QCoreApplication::postEvent(focusProxy()->parentWidget(), &newMe);
    }
    return QWebEngineView::eventFilter(o, e);
}

void WebEngineView::contextMenuEvent(QContextMenuEvent *e)
{
    e->ignore(); // 忽略右键菜单
}

void WebEngineView::wheelEvent(QWheelEvent *e)
{
    const int deltaY = e->angleDelta().y();
    ListWidget *list = findListWidget();
    if (!list)
        return;

    QScrollBar *bar = list->verticalScrollBar();
    const int current = bar->value();
    const int minVal = bar->minimum();
    const int maxVal = bar->maximum();

    int newVal = current - deltaY * 3;
    newVal = qBound(minVal, newVal, maxVal);
    bar->setValue(newVal);
    e->accept();
}

ListWidget *WebEngineView::findListWidget()
{
    QWidget *w = parentWidget();
    if (qobject_cast<TextShow *>(w)) {
        for (int i = 0; i < 3 && w; ++i)
            w = w->parentWidget();
    } else {
        for (int i = 0; i < 4 && w; ++i)
            w = w->parentWidget();
    }
    if (auto *messageWidget = qobject_cast<MessageWidget *>(w))
        return messageWidget->getListWidget();
    else
        return nullptr;
}
