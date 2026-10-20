#include "TitleButton.h"
#include "appcontext.h"

TitleButton::TitleButton(AppContext *appContext, const QString &tipText, int tipOffsetX,
                         int tipOffsetY, QWidget *parent)
    : QPushButton(parent),
      appContext(appContext),
      tipText(tipText),
      tipOffsetX(tipOffsetX),
      tipOffsetY(tipOffsetY)
{
    setMouseTracking(true);
    setCursor(Qt::PointingHandCursor);
    updateTipStartPos();
}

TitleButton::~TitleButton() { }

void TitleButton::setTipText(const QString &tipText)
{
    this->tipText = tipText;
}

void TitleButton::setTipOffset(int x, int y)
{
    tipOffsetX = x;
    tipOffsetY = y;
    updateTipStartPos();
}

void TitleButton::mousePressEvent(QMouseEvent *event)
{
    QPushButton::mousePressEvent(event);
    event->ignore();
}

void TitleButton::mouseMoveEvent(QMouseEvent *event)
{
    QPushButton::mouseMoveEvent(event);
    event->ignore();
}

void TitleButton::mouseReleaseEvent(QMouseEvent *event)
{
    QPushButton::mouseReleaseEvent(event);
    event->ignore();
}

bool TitleButton::event(QEvent *event)
{
    if (event->type() == QEvent::ToolTip) {
        const QPoint pos = mapToGlobal(tipStartPos);
        // 提示字体按按钮所在屏的 DPI 换算为像素字号：初始化时窗口尚未被
        // 系统定位，字号可能按主屏 DPI 算出，实际悬浮在副屏时会偏大；每次
        // 显示前按当前屏重设，保证提示框尺寸与文字匹配
        appContext->applyToolTipFontForScreen(screen());
        QToolTip::showText(pos, tipText, this);
    }

    return QPushButton::event(event);
}

void TitleButton::resizeEvent(QResizeEvent *event)
{
    QPushButton::resizeEvent(event);
    updateTipStartPos();
}

void TitleButton::updateTipStartPos()
{
    tipStartPos = QPoint(rect().topLeft().x() - tipOffsetX, rect().topLeft().y() - tipOffsetY);
}
