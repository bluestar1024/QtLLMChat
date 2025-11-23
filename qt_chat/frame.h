#ifndef FRAME_H
#define FRAME_H

#pragma once
#include <QFrame>
#include <QMouseEvent>

class Frame : public QFrame
{
    Q_OBJECT
public:
    explicit Frame(QWidget *parent = nullptr);
    ~Frame();

protected:
    void mouseMoveEvent(QMouseEvent *event) override;
};

#endif // FRAME_H
