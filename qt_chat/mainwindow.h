#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#pragma once
#include "widget.h"
#include "titlewidget.h"
#include "funwidget.h"
#include "listwidget.h"
#include "texteditfull.h"
#include "splitter.h"
#include "frame.h"
#include "settingwidget.h"
#include "chatrecordswidget.h"
#include "messagethread.h"
#include "messagewidget.h"
#include "itemwidget.h"
#include "messagethread.h"
#include "printlabel.h"
#include "globalvariables.h"

#include <dwmapi.h>
#include <windowsx.h>

#include <QtWidgets/QMainWindow>
#include <QtWidgets/QGraphicsDropShadowEffect>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QListWidgetItem>
#include <QtCore/QUrl>
#include <QtCore/QTimer>
#include <QtCore/QString>
#include <QtGui/QScreen>
#include <QtWidgets/QApplication>
#include <QtCore/QVariant>
#include <QtCore/QPropertyAnimation>
#include <QtCore/QQueue>

#include <QQuickWindow>

enum class RegionEnum : int {
    Left = 0,
    Right,
    Top,
    Bottom,
    LeftTop,
    LeftBottom,
    RightTop,
    RightBottom,
    Title,
    Button,
    Middle
};

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    bool nativeEvent(const QByteArray &eventType, void *message, qintptr *result) override;
    void moveEvent(QMoveEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseDoubleClickEvent(QMouseEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;

private:
    void titleWidgetInit();
    void settingWidgetInit();
    void chatRecordsWidgetInit();
    void checkGraphicsBackend();
    void isItemShowFull(QWidget *widget);
    void regionDivision();
    void UiStretch();
    void UiDrag(QPoint globalPos);
    void textCopy();
    void messageRenewResponse();
    void saveCurChatRecord(bool withholdCurChatFile = false);
    void chatRecordsGenerateItem(QString searchText = "");
    void generateCurChatRecord(bool lastIsToggle = true, bool useThinkExpandList = false);
    void messageWidgetRegenerate();
    void showChatRecords();

    QQuickWindow *window;

    FunWidget *chatFun;
    ListWidget *chatShow;
    TextEditFull *chatInput;
    Widget *chatShowWidget;
    Widget *chatInputWidget;
    Widget *contentWidget;
    QVBoxLayout *chatShowVLayout;
    QVBoxLayout *chatInputVLayout;
    QVBoxLayout *contentVLayout;
    Splitter *splitter;
    Frame *mainWidget;
    QVBoxLayout *mainVLayout;
    TitleWidget *titleWidget;
    QList<MessageWidget *> messageWidgetList;
    QQueue<QString> messageQueue;
    QList<int> thinkTimeLengthList;
    SettingWidget *settingWidget;
    ChatRecordsWidget *chatRecordsWidget;
    QPropertyAnimation *settingAnimationMove;
    QPropertyAnimation *chatRecordsAnimationMove;
    bool settingWidgetIsOpen;
    bool chatRecordsWidgetIsOpen;
    QString curChatFile;
    PrintLabel *emptyTextLabel;
    PrintLabel *textCopyLabel;

    bool isProcessing;
    bool isRegenerate;
    bool isRegenerateFirst;
    bool isSetTexting;
    bool pushButtonIsPress;
    QScreen *lastScreen;
    QScreen *curScreen;
    qreal initDpi, lastDpi, curDpi;
    bool screenChanged;
    QList<bool> thinkExpandedList;
    bool isSending;
    bool isContinueShow;
    bool isScreenMax;
    bool isScreenHalf;
    QRect lastNormalGeometry;
    bool isChangeRectFirst;
    QList<QScreen *> screens;
    RegionEnum regionDir;
    int padding;
    int cursorGlobalX;
    int cursorGlobalY;
    QPoint uiGlobalTL;
    QPoint uiGlobalBR;
    int uiRectWidth;
    int uiRectHeight;
    QPoint pressPosDistanceUiGlobalTL;
    bool isDpiChanged;
    bool mouseLeftButtonIsPress;
    QMap<QString, QVariant> widgetSizeDict;
    bool avoidRepeatSelfFun;
    bool first;
    bool messageSendWidgetIsFinished;

    MessageWidget *messageSendWidget;
    MessageWidget *messageRecvWidget;
    ItemWidget *itemSendWidget;
    ItemWidget *itemRecvWidget;
    QHBoxLayout *itemSendHLayout;
    QHBoxLayout *itemRecvHLayout;
    QListWidgetItem *sendItem;
    QListWidgetItem *recvItem;
    MessageThread *thread;
    QString message;

private slots:
    void onDpiChanged();
    void uiMinimize();
    void uiMaximize();
    void uiClose();
    void settingButtonClicked();
    void chatRecordsUiAnimationMove(const QVariant &value);
    void chatRecordsUiMoveFinished();
    void showSearchRecords();
    void clearAllChatRecords();
    void generateChatRecord(QListWidgetItem *item);
    void onBaseUrlTextChanged(const QString &text);
    void onApiKeyTextChanged(const QString &text);
    void onModelNameTextChanged(const QString &text);
    void onMaxTokensBoxValueChanged(int i);
    void onTopPBoxValueChanged(double d);
    void onTemperatureBoxValueChanged(double d);
    void onMaxTokensSliderValueChanged(int i);
    void onTopPSliderValueChanged(int i);
    void onTemperatureSliderValueChanged(int i);
    void messageWidgetResize(MessageWidget *selfMessageWidget);
    void sendMessage();
    void onExecuteNext();
    void startThread();
    void messageStart();
    void queueMessage(const QString &text);
    void recvMessage(const QString &text);
    void messageFinish();
    void getSetTexting(bool);
    void newChat();
};
#endif // MAINWINDOW_H
