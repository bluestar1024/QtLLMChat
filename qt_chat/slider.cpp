#include "slider.h"

Slider::Slider(QWidget *parent) : QSlider(parent)
{
    resize(340, 26);
    setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Minimum);
    setOrientation(Qt::Horizontal);

    setStyleSheet(R"(
        QSlider::groove:horizontal{
            height: 8px;
            border-radius: 4px;
            background-color: rgb(150, 150, 150);
        }
        QSlider::handle:horizontal{
            width: 26px;
            margin: -9px 0px -9px 0px;
            border-radius: 13px;
            background-color: rgb(50, 50, 50);
        }
        QSlider::handle:hover:horizontal{
            background-color: rgb(70, 70, 70);
        }
        QSlider::sub-page:horizontal{
            border-radius: 4px;
            background-color: rgb(90, 90, 90);
        }
    )");
}

Slider::~Slider() { }

void Slider::mousePressEvent(QMouseEvent *event)
{
    QSlider::mousePressEvent(event);
    event->ignore();
}

void Slider::mouseReleaseEvent(QMouseEvent *event)
{
    QSlider::mouseReleaseEvent(event);
    event->ignore();
}

void Slider::setSize(int width, int height)
{
    if (height % 3 == 1) {
        height = height - 1;
    }
    setFixedSize(width, height);

    int grooveHeight = height / 3;
    int borderRadius = height / 6;
    int handleMargin = -(height - grooveHeight) / 2;
    int handleRadius = height / 2;
    setStyleSheet(QString(R"(
        QSlider::groove:horizontal{
            height: %1px;
            border-radius: %2px;
            background-color: rgb(150, 150, 150);
        }
        QSlider::handle:horizontal{
            width: %3px;
            margin: %4px 0px %4px 0px;
            border-radius: %5px;
            background-color: rgb(50, 50, 50);
        }
        QSlider::handle:hover:horizontal{
            background-color: rgb(70, 70, 70);
        }
        QSlider::sub-page:horizontal{
            border-radius: %2px;
            background-color: rgb(90, 90, 90);
        }
    )")
                          .arg(grooveHeight)
                          .arg(borderRadius)
                          .arg(height)
                          .arg(handleMargin)
                          .arg(handleRadius));
}
