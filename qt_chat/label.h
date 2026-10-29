#ifndef LABEL_H
#define LABEL_H

#pragma once
#include <QtWidgets/QLabel>
#include <QtGui/QFont>

class AppContext;

class Label : public QLabel
{
    Q_OBJECT
public:
    explicit Label(AppContext *appContext, QWidget *parent = nullptr);
    ~Label();

    // 跨屏 DPI 变化时按新像素字号重设字体（像素字号不随屏 DPI 自动变化）
    void updateFontSize();

private:
    AppContext *appContext;
    QString fontFamily;
};

#endif // LABEL_H
