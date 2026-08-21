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

// #include <windowsx.h>
#ifdef Q_OS_WIN
#  include <qt_windows.h>
#  include <Windowsx.h>
#  include <dwmapi.h>
#  pragma comment(lib, "user32.lib")
#  pragma comment(lib, "dwmapi.lib")
#  pragma comment(lib, "gdi32.lib")
#endif

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
#include <QtQuick/QQuickWindow>
#include <QtGui/QWindowStateChangeEvent>

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
    void showEvent(QShowEvent *event) override;
    void changeEvent(QEvent *event) override;
    bool nativeEvent(const QByteArray &eventType, void *message, qintptr *result) override;
    // void paintEvent(QPaintEvent *event) override;
    void moveEvent(QMoveEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    // void mouseDoubleClickEvent(QMouseEvent *event) override;
    // void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;

private:
    void titleWidgetInit();
    void settingWidgetInit();
    void chatRecordsWidgetInit();
    void checkGraphicsBackend();
    void applyDWMShadow();
    void isItemShowFull(QWidget *widget);
    bool isWindowMaximized() const;
    void regionDivision();
    void UiStretch();
    void UiDrag(QPoint globalPos);
    void messageFinish();
    void textCopy();
    void messageRenewResponse();
    void writeToChatRecordFile(bool withholdCurChatFile = false);
    void saveCurChatRecord(bool withholdCurChatFile = false);
    void chatRecordsGenerateItem(QString searchText = "");
    void generateCurChatRecord(bool lastIsToggle = true, bool useThinkExpandList = false);
    void messageWidgetRegenerate();

    QQuickWindow *qwindow;

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
    Widget *mainWidget;
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

    bool isShowFirst;
    bool isProcessing;
    bool isSetTexting;
    bool isRegenerating;
    bool isRegeneratePending;
    bool pushButtonIsPress;
    QScreen *lastScreen;
    QScreen *curScreen;
    qreal initDpi, lastDpi, curDpi;
    bool screenChanged;
    QList<bool> thinkExpandedList;
    bool isSending;
    bool isThreadFinished;
    bool isContinueShow;
    bool isScreenMax;
    bool isScreenHalf;
    QRect lastNormalGeometry;
    int dragStartWidth;
    int dragStartHeight;
    bool isSizeMoveDrag;
    bool dragRegenerateDone;
    bool pendingRegenerateAfterResize;
    int lastRegenerateWidth;
    int lastRegenerateHeight;
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
    int borderLen;

    int currentScrollValue = 0;
    int maxScrollValue = 0;
    QString chatRecordFileName;

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

    MessageWidget *messageWidget = nullptr;
    QHBoxLayout *itemHLayout = nullptr;
    ItemWidget *itemWidget = nullptr;
    QListWidgetItem *item = nullptr;

private slots:
    void onDpiChanged();
    void uiMinimize();
    void uiMaximize();
    void uiClose();
    void settingButtonClicked();
    void chatRecordsUiAnimationMove(const QVariant &value);
    void chatRecordsUiMoveFinished();
    void setScrollValue();
    void showChatRecords();
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
    void onThreadFinished();
    void getSetTexting(bool);
    void newChat();
};
#endif // MAINWINDOW_H
