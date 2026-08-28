#ifndef TEXTSHOW_H
#define TEXTSHOW_H

#pragma once
#include "thinkwidget.h"

class AppContext;

class TextShow : public ThinkWidget
{
    Q_OBJECT
public:
    explicit TextShow(AppContext *appContext, const QString &text = "", std::function<void()> sizeFinishFun = nullptr, std::function<void()> executeNextFun = nullptr, int maxWidth = 810, QWidget *parent = nullptr);
    ~TextShow();

    // template <typename T>
    // void connectExecuteNext(T *receiver, void (T::*slot)());

signals:
    void executeNext();

private slots:
    void onUpdateSize() override;

private:
    std::function<void()> executeNextFun;
    bool firstExecuteNextEmit;
};

// template <typename T>
// void TextShow::connectExecuteNext(T *receiver, void (T::*slot)())
// {
//     connect(this, &TextShow::executeNext, receiver, slot);
// }

#endif // TEXTSHOW_H
