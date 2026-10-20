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
        // 提示字体由 MainWindow::applyToolTipFont 统一设置（像素字号），
        // 此处不可再 setFont(pointSize)，否则提示框尺寸会大于文字所需尺寸
        QToolTip::showText(mapToGlobal(tipStartPos), tipText, this);
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
