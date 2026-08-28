#ifndef SPINBOX_H
#define SPINBOX_H

#pragma once
#include <QtWidgets/QSpinBox>
#include <QtGui/QMouseEvent>

class AppContext;

class SpinBox : public QSpinBox
{
    Q_OBJECT
public:
    explicit SpinBox(AppContext *appContext, QWidget *parent = nullptr);
    ~SpinBox();

    void setSize(int width, int height);

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

private:
    AppContext *appContext;
    QString upArrowImagePath;
    QString downArrowImagePath;
};

#endif // SPINBOX_H
