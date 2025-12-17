#include "loadingwidget.h"

LoadingWidget::LoadingWidget(QWidget *parent) : QWidget(parent), angle(0)
{
    setFixedSize(35, 35);
    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &LoadingWidget::updateAnimation);
    timer->start(1);
}

LoadingWidget::~LoadingWidget() { }

void LoadingWidget::updateAnimation()
{
    angle = (angle + 9) % 360;
    update();
}

void LoadingWidget::paintEvent(QPaintEvent * /*event*/)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    const qreal w = width();
    const qreal h = height();
    const qreal r = (w - 10) / 2.0;

    qreal x1 = w / 2.0 + qCos(qDegreesToRadians(static_cast<qreal>(angle))) * r;
    qreal y1 = h / 2.0 + qSin(qDegreesToRadians(static_cast<qreal>(angle))) * r;
    qreal x2 = w / 2.0 - qCos(qDegreesToRadians(static_cast<qreal>(angle))) * r;
    qreal y2 = h / 2.0 - qSin(qDegreesToRadians(static_cast<qreal>(angle))) * r;

    QLinearGradient gradient(x1, y1, x2, y2);
    gradient.setColorAt(0, QColor(224, 224, 224));
    gradient.setColorAt(1, QColor(100, 100, 100));

    QPen pen;
    pen.setBrush(gradient);
    pen.setWidth(3);
    pen.setCapStyle(Qt::RoundCap);
    painter.setPen(pen);

    QRectF rect = this->rect().adjusted(5, 5, -5, -5);
    painter.drawArc(rect, -angle * 16, -180 * 16);
}
