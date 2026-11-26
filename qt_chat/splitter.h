#ifndef SPLITTER_H
#define SPLITTER_H

#pragma once
#include <QtWidgets/QSplitter>
#include <QtGui/QMouseEvent>

class Splitter : public QSplitter
{
    Q_OBJECT
public:
    explicit Splitter(Qt::Orientation orientation = Qt::Vertical, QWidget *parent = nullptr);
    ~Splitter();

protected:
    void mouseMoveEvent(QMouseEvent *e) override;
    // 高度调节由 TextEditFull 顶部胶囊接管：分隔条不响应鼠标，悬停与拖拽都
    // 不再出现上下调节光标、也不会改变 pane 高度
    QSplitterHandle *createHandle() override;
};

#endif // SPLITTER_H
