#ifndef MESSAGEWIDGET_H
#  define MESSAGEWIDGET_H

#  pragma once
#  include "listwidget.h"
#  include "textshow.h"

#  include <QtWidgets/QWidget>
#  include <QtWidgets/QHBoxLayout>
#  include <QtWidgets/QLabel>
#  include <QtCore/QString>

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

#pragma once
#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QTimer>
#include <QRegularExpression>

QT_BEGIN_NAMESPACE
class TextShow;
class CodeShow;
class ThinkingButton;
class LoadingWidget;
class ImageLabel;
QT_END_NAMESPACE

extern const QString imagesDir;

class MessageWidget : public QWidget
{
    Q_OBJECT
public:
    explicit MessageWidget(const QString &text, std::function<void()> copyFun,
                           std::function<void()> renewResponseFun, bool isUser = true,
                           int textMaxWidth = 877, QWidget *parent = nullptr);
    ~MessageWidget();

    void setText(const QString &text);
    QString getText() const { return text; }
    bool getIsUser() const { return isUser; }

    void setSize();
    void removeLoadingWidget();
    void showFunWidget();
    void hideFunWidget();

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
    void parseThinkAndResult(const QString &txt, QString &think, QString &result, bool &thinkEnd);
    QList<CodeBlock> extractCodeBlocks(const QString &text);
    void updateFunWidgetSize(int curDpi, int initDpi);

    QString copyImagesPath;
    QString copyHoverImagesPath;
    QString renewResponseImagesPath;
    QString renewResponseHoverImagesPath;

    struct CodeBlock
    {
        QString language;
        QString code;
        bool hasEndMarker;
    };

    QString text;
    bool isUser;
    int textMaxWidth;
    std::function<void()> copyFun;
    std::function<void()> renewResponseFun;

    bool thinkButtonHaveCreated = false;
    QString thinkText;
    QString resultText;
    bool thinkTextIsRecvEnd = false;
    bool isRecvFirst = true;
    bool thinkIsExpand = true;

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
