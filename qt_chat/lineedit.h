#ifndef LINEEDIT_H
#define LINEEDIT_H

#pragma once
#include "pushbutton.h"

#include <QtWidgets/QLineEdit>
#include <QtGui/QFont>
#include <QtGui/QFontDatabase>
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
    void updateSize(int curDpi, int lastDpi);

private:
    AppContext *appContext;
    QPushButton *searchButton;
    QFont font;
    QMap<QString, QSize> widgetSizeDict;
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
