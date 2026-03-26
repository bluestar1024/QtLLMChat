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

class MessageWidget : public QWidget
{
    Q_OBJECT
public:
    explicit MessageWidget(const QString &text,
                           std::function<void()> copyFun,
                           std::function<void()> renewResponseFun,
                           std::function<void()> widgetResizeFun,
                           std::function<void(bool)> getSetTextingFun,
                           std::function<void()> executeNextFun,
                           ListWidget *listWidget,
                           QList<int> &thinkTimeLengthList,
                           int thinkTimeIndex,
                           bool isUser = true,
                           bool thinkIsExpand = true,
                           int textMaxWidth = 877,
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
    TextWidget *getTextWidget();
    TextBoxWidget *getTextBoxWidget();
    ListWidget *getListWidget();
    bool hasSelectedText();
    QString getSelectedText();

signals:
    void resizeFinished();
    void setTexting(bool);

private slots:
    void onSizeFinished();
    void onAiUpdateSize();
    void thinkButtonClicked();

private:
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
    std::function<void()> widgetResizeFun;
    std::function<void(bool)> getSetTextingFun;
    std::function<void()> executeNextFun;
    ListWidget *listWidget;
    QList<int> &thinkTimeLengthList;
    int thinkTimeIndex;
    bool isUser;
    bool thinkIsExpand;
    int textMaxWidth;

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
    QTimer aiUpdateSizeTimer;

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
