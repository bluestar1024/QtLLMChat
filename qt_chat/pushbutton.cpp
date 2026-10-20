#include "pushbutton.h"
#include "appcontext.h"

PushButton::PushButton(AppContext *appContext, const QString &tipText, int tipOffsetX,
                       int tipOffsetY, QWidget *parent)
    : QPushButton(parent), appContext(appContext), tipText(tipText)
{
    setCursor(Qt::PointingHandCursor);
    tipStartPos = rect().topLeft() - QPoint(tipOffsetX, tipOffsetY);
}

PushButton::~PushButton() { }

void PushButton::mousePressEvent(QMouseEvent *e)
{
    QPushButton::mousePressEvent(e);
    e->ignore();
}

void PushButton::mouseReleaseEvent(QMouseEvent *e)
{
    QPushButton::mouseReleaseEvent(e);
    e->ignore();
}

bool PushButton::event(QEvent *e)
{
    if (e->type() == QEvent::ToolTip) {
        // 提示字体由 MainWindow::applyToolTipFont 统一设置（像素字号）；此处
        // 不可再 setFont(pointSize)，pointSize 的度量与绘制按不同屏 DPI 换算，
        // 会使提示框尺寸大于文字所需尺寸
        QToolTip::showText(mapToGlobal(tipStartPos), tipText, this);
    }
    return QPushButton::event(e);
}

void PushButton::setTipText(QString tipText)
{
    this->tipText = tipText;
}

QString PushButton::getTipText()
{
    return tipText;
}
