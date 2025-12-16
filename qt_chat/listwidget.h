#ifndef LISTWIDGET_H
#define LISTWIDGET_H

#pragma once
#include <QListWidget>
#include <QScrollBar>
#include <QMouseEvent>
#include <QModelIndex>

class ListWidget : public QListWidget
{
    Q_OBJECT
public:
    explicit ListWidget(QWidget *parent = nullptr);
    ~ListWidget();

protected:
    void scrollTo(const QModelIndex &index, ScrollHint hint = EnsureVisible) override;

    void mouseMoveEvent(QMouseEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

private slots:
    void onScrollBarRangeChanged(int min, int max);
    void onScrollBarValueChanged(int value);

private:
    bool m_scrollAutoChange;
    int m_scrollChangeUplimit;
};

#endif // LISTWIDGET_H
