#ifndef THINKBACKWIDGET_H
#define THINKBACKWIDGET_H

#include <QtWidgets/QWidget>
#include <QtGui/QPainter>
#include <QtGui/QPainterPath>
#include <QtGui/QBrush>
#include <QtGui/QPen>

class ThinkBackWidget : public QWidget
{
    Q_OBJECT
public:
    explicit ThinkBackWidget(QWidget *parent = nullptr);
    ~ThinkBackWidget();

protected:
    void paintEvent(QPaintEvent *event) override;
};

#endif // THINKBACKWIDGET_H
