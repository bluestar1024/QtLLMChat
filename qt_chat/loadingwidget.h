#ifndef LOADINGWIDGET_H
#define LOADINGWIDGET_H

#include <QtWidgets/QWidget>
#include <QtCore/QTimer>
#include <QtGui/QPen>
#include <QtGui/QColor>
#include <QtGui/QLinearGradient>
#include <QtGui/QPainter>
#include <QtMath>

class LoadingWidget : public QWidget
{
    Q_OBJECT
public:
    explicit LoadingWidget(QWidget *parent = nullptr);
    ~LoadingWidget();

protected:
    void paintEvent(QPaintEvent *event) override;

private slots:
    void updateAnimation();

private:
    int angle;
    QTimer *timer;
};

#endif // LOADINGWIDGET_H
