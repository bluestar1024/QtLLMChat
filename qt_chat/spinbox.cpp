#include "spinbox.h"
#include "globalvariables.h"

SpinBox::SpinBox(QWidget *parent) : QSpinBox(parent)
{
    resize(170, 32);
    setCursor(Qt::PointingHandCursor);
    setAlignment(Qt::AlignHCenter);

    upArrowImagePath = imagesDir + "/up_arrow.png";
    downArrowImagePath = imagesDir + "/down_arrow.png";

    setStyleSheet(QString(R"(
        QSpinBox {
            border: 2px solid;
            border-radius: 8px;
            background: transparent;
            font: %1px;
        }
        QSpinBox::up-button {
            width: 16px;
            height: 16px;
            border-image: url("%2");
        }
        QSpinBox::up-button:pressed {
            margin-top: 1px;
        }
        QSpinBox::down-button {
            width: 16px;
            height: 16px;
            border-image: url("%3");
        }
        QSpinBox::down-button:pressed {
            margin-bottom: 1px;
        }
    )")
                          .arg(windowFontPixelSize)
                          .arg(upArrowImagePath)
                          .arg(downArrowImagePath));
}

SpinBox::~SpinBox() { }

void SpinBox::setSize(int width, int height)
{
    setFixedSize(width, height);
    int buttonSize = this->height() / 2;
    setStyleSheet(QString(R"(
        QSpinBox {
            border: 2px solid;
            border-radius: 8px;
            background: transparent;
            font: %2px;
        }
        QSpinBox::up-button {
            width: %1px;
            height: %1px;
            border-image: url("%3");
        }
        QSpinBox::up-button:pressed {
            margin-top: 1px;
        }
        QSpinBox::down-button {
            width: %1px;
            height: %1px;
            border-image: url("%4");
        }
        QSpinBox::down-button:pressed {
            margin-bottom: 1px;
        }
    )")
                          .arg(buttonSize)
                          .arg(windowFontPixelSize)
                          .arg(upArrowImagePath)
                          .arg(downArrowImagePath));
}

void SpinBox::mousePressEvent(QMouseEvent *event)
{
    QSpinBox::mousePressEvent(event);
    event->ignore();
}

void SpinBox::mouseReleaseEvent(QMouseEvent *event)
{
    QSpinBox::mouseReleaseEvent(event);
    event->ignore();
}
