#ifndef MESSAGEWIDGET_H
#define MESSAGEWIDGET_H

#pragma once
#include "listwidget.h"
#include "thinkingbutton.h"
#include "thinkbackwidget.h"
#include "textshow.h"
#include "codeshow.h"
#include "textwidget.h"
#include "loadingwidget.h"
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

extern const QString imagesDir;

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

    template <typename T>
    void connectResizeFinished(T *receiver, void (T::*slot)());
    template <typename T>
    void connectSetTexting(T *receiver, void (T::*slot)(bool));
    template <typename T>
    void connectExecuteNext(T *receiver, void (T::*slot)());
    void toggleWidget();
    void breakHandle();
    void removeRenewResponseButton();
    void removeLoadingWidget();
    void showFunWidget();
    void hideFunWidget();

    void setSize();
    void setText(const QString &text);
    QString getText() const { return text; }
    bool getIsUser() const { return isUser; }
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
    void buildUserUi();
    void buildAiUi();
    QList<CodeBlock> extractCodeBlocks(const QString &text);
    void parseThinkAndResult(const QString &txt, QString &think, QString &result, bool &thinkEnd);
    void adjustAiTextWidgetSize();
    void updateFunWidgetSize(int curDpi, int initDpi);

    QString copyImagesPath;
    QString copyHoverImagesPath;
    QString renewResponseImagesPath;
    QString renewResponseHoverImagesPath;

    struct CodeBlock
    {
        QString language;
        QString code;
        QString endMarker;
    };

    QString text;
    std::function<void()> copyFun;
    std::function<void()> renewResponseFun;
    ListWidget *listWidget;
    QList<int> &thinkTimeLengthList;
    int thinkTimeIndex;
    bool isUser;
    bool thinkIsExpand = true;
    int textMaxWidth;

    bool thinkButtonHaveCreated = false;
    QString thinkText;
    QString resultText;
    bool thinkTextIsRecvEnd = false;
    bool isRecvFirst = true;

    ImageLabel *imageLabel = nullptr;
    TextWidget *textWidget = nullptr;
    QVBoxLayout *textLayout = nullptr;
    TextBoxWidget *textBoxWidget = nullptr;
    QVBoxLayout *textBoxLayout = nullptr;

    QWidget *funWidget = nullptr;
    CopyButton *copyButton = nullptr;
    PushButton *renewResponseButton = nullptr;
    bool funWidgetIsShow = false;
    bool loadingWidgetIsRemove = true;
    bool renewResponseButtonIsRemove = true;

    ThinkingButton *thinkButton = nullptr;
    ThinkBackWidget *thinkBackWidget = nullptr;
    QVBoxLayout *thinkBackVLayout = nullptr;
    LoadingWidget *loadingWidget = nullptr;

    QList<TextShow *> thinkTextShowList;
    QList<CodeShow *> thinkCodeShowList;
    QList<TextShow *> resultTextShowList;
    QList<CodeShow *> resultCodeShowList;

    QTimer aiUpdateSizeTimer;
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
