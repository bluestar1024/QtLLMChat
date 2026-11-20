#ifndef SETTINGEDIT_H
#define SETTINGEDIT_H

#pragma once
#include <QtWidgets/QLineEdit>

class AppContext;

class SettingEdit : public QLineEdit
{
    Q_OBJECT
public:
    explicit SettingEdit(AppContext *appContext, QWidget *parent = nullptr);
    ~SettingEdit();

    // 只设定高度：宽度由所在行的 QHBoxLayout 按剩余空间分配。若手工按行宽
    // 逐个拼算固定宽度，多轮 DPI 缩放后标签宽度、间距与行宽的取整误差互不
    // 抵消，输入框会顶到行的右边缘，右框线被父部件裁掉
    void setHeight(int h);
    // 跨屏 DPI 变化时重应用样式表（内含像素字号 font-size）
    void updateFontSize();

private:
    void updateStyleSheet();

    AppContext *appContext;
};

#endif // SETTINGEDIT_H
