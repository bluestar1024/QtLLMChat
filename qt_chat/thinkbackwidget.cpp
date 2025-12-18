#include "thinkbackwidget.h"

ThinkBackWidget::ThinkBackWidget(QWidget *parent) : QWidget{ parent } { }

ThinkBackWidget::~ThinkBackWidget() { }

void ThinkBackWidget::paintEvent(QPaintEvent * /*event*/)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    // 背景圆角矩形
    QPainterPath backgroundPath;
    backgroundPath.addRoundedRect(rect(), 13, 13);

    QBrush backgroundBrush(Qt::SolidPattern);
    backgroundBrush.setColor(QColor(244, 244, 252));

    painter.setPen(Qt::NoPen);
    painter.setBrush(backgroundBrush);
    painter.drawPath(backgroundPath.simplified());

    // 左侧竖线
    QPainterPath path;
    path.addRect(rect().x() + 14, rect().y() + 12, 2, rect().height() - 24);

    QBrush brush(Qt::SolidPattern);
    brush.setColor(QColor(172, 188, 220));

    painter.setPen(Qt::NoPen);
    painter.setBrush(brush);
    painter.drawPath(path.simplified());
}
