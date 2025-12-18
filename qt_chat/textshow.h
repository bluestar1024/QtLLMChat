#ifndef TEXTSHOW_H
#define TEXTSHOW_H

#pragma once
#include "thinkwidget.h"

class TextShow : public ThinkWidget
{
    Q_OBJECT
public:
    explicit TextShow(const QString &text = "", int maxWidth = 810, QWidget *parent = nullptr);
    ~TextShow();

    template <typename T>
    void connectExecuteNext(T *receiver, void (T::*slot)());

signals:
    void executeNext();

private slots:
    void onUpdateSize() override;

private:
    bool firstExecuteNextEmit;
};

template <typename T>
void TextShow::connectExecuteNext(T *receiver, void (T::*slot)())
{
    connect(this, &TextShow::executeNext, receiver, slot);
}

#endif // TEXTSHOW_H
