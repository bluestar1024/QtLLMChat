#include "copybutton.h"
#include "messagewidget.h"
#include "appcontext.h"

CopyButton::CopyButton(AppContext *appContext, const QString &tipText, QWidget *parent)
    : QPushButton(parent), appContext(appContext), tipText(tipText)
{
    setCursor(Qt::PointingHandCursor);
}

CopyButton::~CopyButton() { }

void CopyButton::mousePressEvent(QMouseEvent *e)
{
    qDebug() << "parent:" << parent() << "qobject_cast<MessageWidget *>(parent()):"
             << qobject_cast<MessageWidget *>(parent());
    QString text;
    // if (auto *messageWidget = qobject_cast<MessageWidget *>(parent())) {
    if (auto *messageWidget = getMessageWidget()) {
        if (messageWidget->hasSelectedText())
            text = messageWidget->getSelectedText();
        else
            text = messageWidget->getText();
        if (!text.isEmpty())
            QApplication::clipboard()->setText(text);
        // QPushButton::mousePressEvent(e);
        // e->ignore();
    }
    QPushButton::mousePressEvent(e);
    e->ignore();
    qDebug() << "复制文本：" << text;
}

void CopyButton::mouseReleaseEvent(QMouseEvent *e)
{
    QPushButton::mouseReleaseEvent(e);
    e->ignore();
}

bool CopyButton::event(QEvent *e)
{
    if (e->type() == QEvent::ToolTip) {
        // 提示字体按按钮所在屏的 DPI 换算为像素字号：初始化时窗口尚未被
        // 系统定位，字号可能按主屏 DPI 算出，实际悬浮在副屏时会偏大；每次
        // 显示前按当前屏重设，保证提示框尺寸与文字匹配
        appContext->applyToolTipFontForScreen(screen());
        // 提示位置：水平中心与按钮中心对齐（左右对称），竖直位于按钮上方
        // 且不遮挡按钮与鼠标（详见 AppContext::toolTipPos）
        const QRect buttonRect(mapToGlobal(QPoint(0, 0)), size());
        QToolTip::showText(appContext->toolTipPos(buttonRect, tipText), tipText, this);
    }
    return QPushButton::event(e);
}

MessageWidget *CopyButton::getMessageWidget()
{
    QObject *w = qobject_cast<QObject *>(this);
    do {
        w = w->parent();
        if (!w)
            break;
    } while (!qobject_cast<MessageWidget *>(w));
    return qobject_cast<MessageWidget *>(w);
}
