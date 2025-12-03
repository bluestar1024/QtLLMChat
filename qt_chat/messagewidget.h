#ifndef MESSAGEWIDGET_H
#define MESSAGEWIDGET_H

#pragma once
#include <QWidget>
#include <QString>
#include <QHBoxLayout>
#include <QLabel>
#include "listwidget.h"
#include "textshow.h"

class MessageWidget : public QWidget
{
    Q_OBJECT
public:
    explicit MessageWidget(const QString &text,
                           std::function<void()> copyFun,
                           std::function<void()> renewResponseFun,
                           ListWidget *listWidget,
                           QList<int> &thinkTimeLengthList,
                           int thinkTimeIndex,
                           bool isUser = true,
                           bool thinkIsExpand = true,
                           int textMaxWidth = 877,
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
    void setText(const QString &text);
    void removeLoadingWidget();
    ListWidget *getListWidget();

signals:
    void resizeFinished();
    void setTexting(bool state);

private:
    void onSizeFinshed();

    QString m_text;
    std::function<void()> m_copyFun;
    std::function<void()> m_renewResponseFun;
    ListWidget *m_listWidget = nullptr;
    QList<int> &m_thinkTimeLengthList;
    int  m_thinkTimeIndex;
    bool m_isUser;
    bool m_thinkIsExpand;
    int  m_textMaxWidth;

    QHBoxLayout *m_layout;
    TextShow *m_textShow;
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
    //    connect(this, &MessageWidget::executeNext, receiver, slot);
    if (m_isUser) m_textShow->connectExecuteNext(receiver, slot);
}

#endif // MESSAGEWIDGET_H
