#ifndef SPLITTER_H
#define SPLITTER_H

#pragma once
#include <QSplitter>
#include <QMouseEvent>

class Splitter : public QSplitter
{
    Q_OBJECT
public:
    explicit Splitter(Qt::Orientation orientation = Qt::Vertical, QWidget *parent = nullptr);
    ~Splitter();

protected:
    void mouseMoveEvent(QMouseEvent *event) override;
};

#endif // SPLITTER_H
