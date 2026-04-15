#ifndef SLIDER_H
#define SLIDER_H

#pragma once
#include <QtWidgets/QSlider>
#include <QtGui/QMouseEvent>
#include <QtWidgets/QSizePolicy>

class Slider : public QSlider
{
    Q_OBJECT
public:
    explicit Slider(QWidget *parent = nullptr);
    ~Slider();

    void setSize(QSize s);

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
};

#endif // SLIDER_H
