#ifndef DOUBLESPINBOX_H
#define DOUBLESPINBOX_H

#pragma once
#include <QtWidgets/QDoubleSpinBox>
#include <QtGui/QMouseEvent>

class DoubleSpinBox : public QDoubleSpinBox
{
    Q_OBJECT
public:
    explicit DoubleSpinBox(QWidget *parent = nullptr);
    ~DoubleSpinBox();

    void setSize(int width, int height);

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

private:
    QString upArrowImagePath;
    QString downArrowImagePath;
};

#endif // DOUBLESPINBOX_H
