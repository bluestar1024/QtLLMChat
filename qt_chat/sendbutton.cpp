#include "sendbutton.h"
#include "appcontext.h"

SendButton::SendButton(AppContext *appContext, const QString &tipText, int tipOffsetX,
                       int tipOffsetY, QWidget *parent)
    : QPushButton(parent), appContext(appContext), tipText(tipText)
{
    setCursor(Qt::PointingHandCursor);
    tipStartPos = rect().topLeft() - QPoint(tipOffsetX, tipOffsetY);
}

SendButton::~SendButton() { }

bool SendButton::event(QEvent *e)
{
    if (e->type() == QEvent::ToolTip) {
        const QPoint pos = mapToGlobal(tipStartPos);
        // 提示字体按按钮所在屏的 DPI 换算为像素字号：初始化时窗口尚未被
        // 系统定位，字号可能按主屏 DPI 算出，实际悬浮在副屏时会偏大；每次
        // 显示前按当前屏重设，保证提示框尺寸与文字匹配
        appContext->applyToolTipFontForScreen(screen());
        QToolTip::showText(pos, tipText, this);
    }
    return QPushButton::event(e);
}
