#include "textwidget.h"

TextWidget::TextWidget(bool isUser, QWidget *parent) : QWidget{ parent }, isUser(isUser)
{
    setMouseTracking(true);
}

TextWidget::~TextWidget() { }

void TextWidget::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    QPainterPath path;
    const QRect r = rect();
    path.addRoundedRect(r.x(), r.y(), r.width(), r.height(), 13, 13);

    QBrush brush(Qt::SolidPattern);
    if (isUser)
        brush.setColor(QColor(235, 243, 251));
    else
        brush.setColor(Qt::white);

    painter.setPen(Qt::NoPen);
    painter.setBrush(brush);
    painter.drawPath(path.simplified());
}
