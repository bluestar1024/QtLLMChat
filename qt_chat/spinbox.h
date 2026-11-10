#ifndef SPINBOX_H
#define SPINBOX_H

#pragma once
#include <QtGui/QMouseEvent>
#include <QtGui/QResizeEvent>
#include <QtWidgets/QSpinBox>

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
    void resizeEvent(QResizeEvent *event) override;

private:
    // 按“144 DPI 基准尺寸 × 当前屏 DPI 比例”重算边框、圆角与上下按钮尺寸；
    // 箭头图标由样式表的 image 属性绘制（等比缩放到按钮内并居中）
    void applyStyleSheet();

    AppContext *appContext;
    // 最近一次应用样式表时的控件高度、像素字号与 DPI 比例：三者都不变时
    // 跳过，避免 resizeEvent 中间接改样式表引起的重复布局
    int styledButtonHeight;
    int styledFontPixelSize;
    int styledDpiScale;
};

#endif // SPINBOX_H
