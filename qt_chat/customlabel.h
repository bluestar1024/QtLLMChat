#ifndef CUSTOMLABEL_H
#define CUSTOMLABEL_H

#pragma once
#include <QtWidgets/QLabel>
#include <QtGui/QMouseEvent>
#include <QtGui/QContextMenuEvent>

class CustomLabel : public QLabel
{
    Q_OBJECT
public:
    explicit CustomLabel(QWidget *parent = nullptr);
    ~CustomLabel();

protected:
    void mousePressEvent(QMouseEvent *e) override;
    void mouseReleaseEvent(QMouseEvent *e) override;
    void contextMenuEvent(QContextMenuEvent *e) override;
};

#endif // CUSTOMLABEL_H
