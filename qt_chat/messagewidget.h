#ifndef MESSAGEWIDGET_H
#define MESSAGEWIDGET_H

#include <QWidget>
#include <QString>
#include <QListWidget>
#include <QHBoxLayout>
#include <QLabel>

class MessageWidget : public QWidget
{
    Q_OBJECT
public:
    explicit MessageWidget(const QString &text,
                           std::function<void()> copyFun,
                           std::function<void()> renewResponseFun,
                           QListWidget *listWidget,
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

signals:
    void resizeFinished();
    void setTexting(bool state);

    //
    void executeNext();

private:
    QString m_text;
    std::function<void()> m_copyFun;
    std::function<void()> m_renewResponseFun;
    QListWidget *m_listWidget;
    QList<int> &m_thinkTimeLengthList;
    int  m_thinkTimeIndex;
    bool m_isUser;
    bool m_thinkIsExpand;
    int  m_textMaxWidth;

    QHBoxLayout *layout;
    QLabel *label;
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
    connect(this, &MessageWidget::executeNext, receiver, slot);
}

#endif // MESSAGEWIDGET_H
