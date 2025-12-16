#ifndef LISTWIDGET_H
#define LISTWIDGET_H

#pragma once
#include <QtWidgets/QListWidget>
#include <QtWidgets/QScrollBar>
#include <QtGui/QMouseEvent>
#include <QtCore/QModelIndex>

class ListWidget : public QListWidget
{
    Q_OBJECT
public:
    explicit ListWidget(QWidget *parent = nullptr);
    ~ListWidget();

protected:
    void scrollTo(const QModelIndex &index, ScrollHint hint = EnsureVisible) override;

    void mouseMoveEvent(QMouseEvent *e) override;
    void mousePressEvent(QMouseEvent *e) override;
    void mouseReleaseEvent(QMouseEvent *e) override;

private slots:
    void onScrollBarRangeChanged(int min, int max);
    void onScrollBarValueChanged(int value);

private:
    bool scrollAutoChange;
    int scrollChangeUplimit;
};

#endif // LISTWIDGET_H
