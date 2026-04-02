#include "doublespinbox.h"
#include "globalvariables.h"

DoubleSpinBox::DoubleSpinBox(QWidget *parent) : QDoubleSpinBox(parent)
{
    resize(170, 32);
    setCursor(Qt::PointingHandCursor);
    setAlignment(Qt::AlignHCenter);

    upArrowImagePath = QDir(imagesDir).filePath("up_arrow.png");
    downArrowImagePath = QDir(imagesDir).filePath("down_arrow.png");

    setStyleSheet(QString(R"(
        QDoubleSpinBox {
            border: 2px solid;
            border-radius: 8px;
            background: transparent;
            font: %1px;
        }
        QDoubleSpinBox::up-button {
            width: 16px;
            height: 16px;
            border-image: url("%2");
        }
        QDoubleSpinBox::up-button:pressed {
            margin-top: 1px;
        }
        QDoubleSpinBox::down-button {
            width: 16px;
            height: 16px;
            border-image: url("%3");
        }
        QDoubleSpinBox::down-button:pressed {
            margin-bottom: 1px;
        }
    )")
                          .arg(windowFontPixelSize)
                          .arg(upArrowImagePath)
                          .arg(downArrowImagePath));
}

DoubleSpinBox::~DoubleSpinBox() { }

void DoubleSpinBox::setSize(int width, int height)
{
    setFixedSize(width, height);
    int buttonSize = this->height() / 2;
    setStyleSheet(QString(R"(
        QDoubleSpinBox {
            border: 2px solid;
            border-radius: 8px;
            background: transparent;
            font: %2px;
        }
        QDoubleSpinBox::up-button {
            width: %1px;
            height: %1px;
            border-image: url("%3");
        }
        QDoubleSpinBox::up-button:pressed {
            margin-top: 1px;
        }
        QDoubleSpinBox::down-button {
            width: %1px;
            height: %1px;
            border-image: url("%4");
        }
        QDoubleSpinBox::down-button:pressed {
            margin-bottom: 1px;
        }
    )")
                          .arg(buttonSize)
                          .arg(windowFontPixelSize)
                          .arg(upArrowImagePath)
                          .arg(downArrowImagePath));
}

void DoubleSpinBox::mousePressEvent(QMouseEvent *event)
{
    QDoubleSpinBox::mousePressEvent(event);
    event->ignore();
}

void DoubleSpinBox::mouseReleaseEvent(QMouseEvent *event)
{
    QDoubleSpinBox::mouseReleaseEvent(event);
    event->ignore();
}
