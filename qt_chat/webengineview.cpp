#include "webengineview.h"
#include "messagewidget.h"
#include "appcontext.h"

WebEngineView::WebEngineView(AppContext *appContext, QWidget *parent)
    : QWebEngineView(parent), appContext(appContext)
{
    setPage(new WebEnginePage(this->appContext->webEngineProfile(), this));
    page()->setBackgroundColor(Qt::transparent);
    // 不在构造中触发引擎初始化（原为 load(QUrl())）：此时视图仍是 QWidget 默认尺寸
    // 100x30，渲染视口会被锁定为该尺寸；而控件显示前收不到真实 resize 事件（Qt 对
    // 隐藏控件延迟 resize 事件且不激活内部布局），视口不再更新，runJavaScript 量测
    // 恒得 100 宽，导致从历史记录重建消息时宽度错误。改由首次 setHtml 惰性初始化：
    // 此时调用方（ThinkWidget）已按内容 setFixedWidth，初始化即以正确尺寸建立视口。
    // focusProxy()->setAttribute(Qt::WA_TransparentForMouseEvents);
    // setFocusPolicy(Qt::NoFocus);
}

WebEngineView::~WebEngineView() { }

// 渲染 delegate（内部 WebEngineQuickWidget，派生自 QQuickWidget，且无独立元对象，
// inherits 按基类名匹配）只在引擎初始化时创建并挂载到本视图，构造期不存在，无法在
// 构造函数中安装事件过滤器。在其加入时安装，保持鼠标释放事件转发行为（widgetChanged
// 中 setFocusProxy 指向同一控件，eventFilter 内的 o == focusProxy() 判断因此成立）。
void WebEngineView::childEvent(QChildEvent *e)
{
    QWebEngineView::childEvent(e);
    if (e->added() && e->child()->isWidgetType() && e->child()->inherits("QQuickWidget"))
        e->child()->installEventFilter(this);
}

bool WebEngineView::eventFilter(QObject *o, QEvent *e)
{
    qDebug() << "WebEngineView eventFilter before:" << e->type();
    if (o == focusProxy() && e->type() == QEvent::MouseButtonRelease) {
        auto *me = static_cast<QMouseEvent *>(e);
        // QMouseEvent newMe(me->type(), me->position(), me->globalPosition(), me->button(),
        //                   me->buttons(), me->modifiers());
        // QCoreApplication::postEvent(focusProxy()->parentWidget(), &newMe);
        QMouseEvent *newMe = new QMouseEvent(me->type(), me->position(), me->globalPosition(),
                                             me->button(), me->buttons(), me->modifiers());
        QCoreApplication::postEvent(focusProxy()->parentWidget(), newMe);
        // QCoreApplication::sendEvent(focusProxy()->parentWidget(), newMe);
        // return true;
    }
    qDebug() << "WebEngineView eventFilter after:" << e->type();
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
