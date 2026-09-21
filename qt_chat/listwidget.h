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

    // 恢复自动跟随：滚动条距底部超过阈值时自动跟随会关闭（避免浏览历史时被打断），
    // 开始新消息/新建聊天/切换聊天记录时需恢复，否则新内容追加时
    // 滚动条不再自动滚动到底部（用户视角：滚动条停留在原位置不动）
    void resetScrollAutoChange();

protected:
    void scrollTo(const QModelIndex &, ScrollHint = EnsureVisible) override;

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
