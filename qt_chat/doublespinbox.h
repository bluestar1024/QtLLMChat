#ifndef DOUBLESPINBOX_H
#define DOUBLESPINBOX_H

#pragma once
#include <QtGui/QMouseEvent>
#include <QtGui/QResizeEvent>
#include <QtWidgets/QDoubleSpinBox>

class AppContext;

class DoubleSpinBox : public QDoubleSpinBox
{
    Q_OBJECT
public:
    explicit DoubleSpinBox(AppContext *appContext, QWidget *parent = nullptr);
    ~DoubleSpinBox();

    void setSize(int width, int height);

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;

private:
    // 按“144 DPI 基准尺寸 × 当前屏 DPI 比例”重算边框、圆角与上下按钮尺寸；
    // 箭头图标由样式表的 image 属性绘制在按钮内容区内，内容区宽度按箭头长宽比
    // 由高度反推，padding 提供“靠控件外缘大、靠中线小”两档随 DPI 缩放的留白
    void applyStyleSheet();

    AppContext *appContext;
    // 最近一次应用样式表时的控件宽高、像素字号与 DPI 比例：全不变时跳过，
    // 避免 resizeEvent 中间接改样式表引起的重复布局。宽度也要比：内容区宽度
    // 会被“控件太窄”的限幅压小，宽度变化时箭头宽度得重新算
    int styledWidth;
    int styledHeight;
    int styledFontPixelSize;
    int styledDpiScale;
};

#endif // DOUBLESPINBOX_H
