#ifndef MESSAGEWIDGET_H
#define MESSAGEWIDGET_H

#pragma once
#include "listwidget.h"
#include "thinkingbutton.h"
#include "thinkbackwidget.h"
#include "thinkwidget.h"
#include "textshow.h"
#include "codeshow.h"
#include "textwidget.h"
#include "loadingwidget.h"
// #include "funwidget.h"
#include "copybutton.h"
#include "pushbutton.h"
#include "textboxwidget.h"
#include "imagelabel.h"

#include <QtWidgets/QWidget>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QLabel>
#include <QtCore/QTimer>
#include <QtCore/QRegularExpression>
#include <QtCore/QMetaObject>
// #include <QtCore/QQueue>

// extern const QString imagesDir;

class AppContext;

class MessageWidget : public QWidget
{
    Q_OBJECT
public:
    explicit MessageWidget(AppContext *appContext, const QString &text,
                           std::function<void()> copyFun,
                           std::function<void()> renewResponseFun,
                           std::function<void(MessageWidget *)> widgetResizeFun,
                           std::function<void(bool)> getSetTextingFun,
                           std::function<void()> executeNextFun, ListWidget *listWidget,
                           QList<int> &thinkTimeLengthList, int thinkTimeIndex, bool isUser = true,
                           bool thinkIsExpand = true, int textMaxWidth = 877,
                           QWidget *parent = nullptr);
    ~MessageWidget();

    // template <typename T>
    // void connectResizeFinished(T *receiver, void (T::*slot)());
    // template <typename T>
    // void connectSetTexting(T *receiver, void (T::*slot)(bool));
    // template <typename T>
    // void connectExecuteNext(T *receiver, void (T::*slot)());
    // void toggleWidget();
    void breakHandle();
    void removeRenewResponseButton();
    void removeLoadingWidget();
    void updateFunWidgetSize(qreal curDpi, qreal initDpi);
    bool getThinkIsExpanded();
    void showFunWidget();
    void hideFunWidget();

    void setSize();
    void setText(const QString &text);
    QString getText();
    bool getIsUser();
    // 构造时确定的实际最大宽度（= 构造参数 - 10），供重建后自检比对是否采样到旧宽度
    int getTextMaxWidth() const { return textMaxWidth; }
    TextWidget *getTextWidget();
    TextBoxWidget *getTextBoxWidget();
    bool getIsRemoveloadingWidget();
    void syncThinkTimeLength();
    ListWidget *getListWidget();
    bool hasSelectedText();
    QString getSelectedText();

signals:
    void resizeFinished(MessageWidget *);
    void setTexting(bool);

private slots:
    void onSizeFinished();
    void thinkButtonClicked();

private:
    AppContext *appContext;

    struct CodeBlock
    {
        QString language;
        QString code;
        QString endMarker;
    };

    void buildUserUi();
    void buildAiUi();
    QList<CodeBlock> extractCodeBlocks(const QString &text);
    void parseThinkAndResult(const QString &txt, QString &think, QString &result, bool &thinkEnd);
    void adjustAiTextWidgetSize();

    QString copyImagesPath;
    QString copyHoverImagesPath;
    QString renewResponseImagesPath;
    QString renewResponseHoverImagesPath;

    QString text;
    std::function<void()> copyFun;
    std::function<void()> renewResponseFun;
    std::function<void(MessageWidget *)> widgetResizeFun;
    std::function<void(bool)> getSetTextingFun;
    std::function<void()> executeNextFun;
    ListWidget *listWidget;
    QList<int> &thinkTimeLengthList;
    int thinkTimeIndex;
    bool isUser;
    bool thinkIsExpand;
    int textMaxWidth;
    // CodeShow 的实际宽度为传入的最大宽度 + 2（内部边框），而 ThinkWidget/TextShow 的宽度
    // 等于传入的最大宽度：代码块的最大宽度需再减去该值，使同一条 AI 消息中代码块与
    // 文本的最大宽度一致，从而 MessageWidget 的最大宽度恒为构造参数 textMaxWidth - 10
    static constexpr int codeShowExtraWidth = 2;

    ImageLabel *imageLabel;
    TextShow *textShow;
    TextWidget *textWidget;
    QVBoxLayout *textLayout;
    TextBoxWidget *textBoxWidget;
    QVBoxLayout *textBoxLayout;
    ThinkingButton *thinkButton;
    ThinkBackWidget *thinkBackWidget;
    QVBoxLayout *thinkBackVLayout;
    QVBoxLayout *subVLayout1;
    QVBoxLayout *subVLayout2;
    QHBoxLayout *mainHLayout;
    LoadingWidget *loadingWidget;
    QWidget *funWidget;
    QHBoxLayout *funHLayout;
    CopyButton *copyButton;
    PushButton *renewResponseButton;

    bool thinkButtonHaveCreated;
    QString thinkText;
    QString resultText;
    bool thinkTextIsRecvEnd;
    bool isRecvFirst;
    bool funWidgetIsShow;
    bool loadingWidgetIsRemove;
    bool renewResponseButtonIsRemove;

    QEventLoop loop;
    QTimer checkTimer;
    // QQueue<QTimer *> processQueue;

    QList<ThinkWidget *> thinkTextShowList;
    QList<CodeShow *> thinkCodeShowList;
    QList<TextShow *> resultTextShowList;
    QList<CodeShow *> resultCodeShowList;
};

// template <typename T>
// void MessageWidget::connectResizeFinished(T *receiver, void (T::*slot)())
// {
//     connect(this, &MessageWidget::resizeFinished, receiver, slot);
// }

// template <typename T>
// void MessageWidget::connectSetTexting(T *receiver, void (T::*slot)(bool))
// {
//     connect(this, &MessageWidget::setTexting, receiver, slot);
// }

// template <typename T>
// void MessageWidget::connectExecuteNext(T *receiver, void (T::*slot)())
// {
//     if (isUser)
//         textShow->connectExecuteNext(receiver, slot);
// }

#endif // MESSAGEWIDGET_H
