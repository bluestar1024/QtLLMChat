#ifndef LINEEDIT_H
#define LINEEDIT_H

#pragma once
#include "pushbutton.h"

#include <QtWidgets/QLineEdit>
#include <QtGui/QFont>
#include <QtGui/QIcon>
#include <QtCore/QSize>
#include <QtCore/QMap>

class AppContext;

class LineEdit : public QLineEdit
{
    Q_OBJECT
public:
    explicit LineEdit(AppContext *appContext, QWidget *parent = nullptr);
    ~LineEdit();

    template <typename T>
    void connectSearchButtonClick(T *receiver, void (T::*slot)());
    template <typename T>
    void connectSearchButtonClick(T *receiver, void (T::*slot)(bool));

    void resetWidgetSize();
    // 按 144 DPI 基准（DpiUtil::kBaseDpi）重设自身与搜索按钮尺寸
    void updateSize(qreal curDpi, qreal lastDpi);

private:
    AppContext *appContext;
    QPushButton *searchButton;
    QFont font;
    // 尺寸缓存以 144 DPI 基准保存（取用时按目标 DPI 换算）
    QMap<QString, QSize> widgetSizeDict;
    qreal appliedDpi;
};

template <typename T>
void LineEdit::connectSearchButtonClick(T *receiver, void (T::*slot)())
{
    connect(searchButton, &QPushButton::clicked, [=](bool) { (receiver->*slot)(); });
}

template <typename T>
void LineEdit::connectSearchButtonClick(T *receiver, void (T::*slot)(bool))
{
    connect(searchButton, &QPushButton::clicked, receiver, slot);
}

#endif // LINEEDIT_H
