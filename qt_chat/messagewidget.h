#ifndef MESSAGEWIDGET_H
#define MESSAGEWIDGET_H

#pragma once
#include "listwidget.h"
#include "textshow.h"

#include <QtWidgets/QWidget>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtCore/QString>

class MessageWidget : public QWidget
{
    Q_OBJECT
public:
    explicit MessageWidget(const QString &text, std::function<void()> copyFun,
                           std::function<void()> renewResponseFun, ListWidget *listWidget,
                           QList<int> &thinkTimeLengthList, int thinkTimeIndex, bool isUser = true,
                           bool thinkIsExpand = true, int textMaxWidth = 877,
                           QWidget *parent = nullptr);
    ~MessageWidget();

    bool getIsUser();
    QString getText();
    void updateFunWidgetSize(qreal curDpi, qreal initDpi);
    template <typename T>
    void connectResizeFinished(T *receiver, void (T::*slot)());
    template <typename T>
    void connectSetTexting(T *receiver, void (T::*slot)(bool));
    template <typename T>
    void connectExecuteNext(T *receiver, void (T::*slot)());
    void toggleWidget();
    void breakHandle();
    void removeRenewResponseButton();
    void setSize();
    void setText(const QString &text);
    void removeLoadingWidget();
    ListWidget *getListWidget();
    bool hasSelectedText();
    QString getSelectedText();

signals:
    void resizeFinished();
    void setTexting(bool state);

private:
    void onSizeFinshed();

    QString text;
    std::function<void()> copyFun;
    std::function<void()> renewResponseFun;
    ListWidget *listWidget = nullptr;
    QList<int> &thinkTimeLengthList;
    int thinkTimeIndex;
    bool isUser;
    bool thinkIsExpand;
    int textMaxWidth;

    QHBoxLayout *layout;
    TextShow *textShow;
};

template <typename T>
void MessageWidget::connectResizeFinished(T *receiver, void (T::*slot)())
{
    connect(this, &MessageWidget::resizeFinished, receiver, slot);
}

template <typename T>
void MessageWidget::connectSetTexting(T *receiver, void (T::*slot)(bool))
{
    connect(this, &MessageWidget::setTexting, receiver, slot);
}

template <typename T>
void MessageWidget::connectExecuteNext(T *receiver, void (T::*slot)())
{
    if (isUser)
        textShow->connectExecuteNext(receiver, slot);
}

#endif // MESSAGEWIDGET_H
