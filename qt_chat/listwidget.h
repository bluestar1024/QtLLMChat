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

    // 新增消息尺寸量测完成前挂起自动滚动：发送时 item 先以未量测的临时高度
    // 入列（真实高度可能只有其五分之一），范围异常增大后若立即 setValue(max)，
    // 会先滚到"假底部"——视口内只剩尚未显示的空白 item 区（视觉上所有消息
    // 消失再出现）。量测完成后由 MessageWidget 的 resizeFinished 链路解除挂起，
    // 随后的真实 rangeChanged 再执行自动跟随
    void setPendingSendItem(bool pending);

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
    bool pendingSendItem;
};

#endif // LISTWIDGET_H
