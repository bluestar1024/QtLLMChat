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
        // 提示字体由 MainWindow::applyToolTipFont 统一设置（像素字号），
        // 此处不可再 setFont(pointSize)，否则提示框尺寸会大于文字所需尺寸
        QToolTip::showText(mapToGlobal(tipStartPos), tipText, this);
    }
    return QPushButton::event(e);
}
