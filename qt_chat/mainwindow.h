#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#pragma once
#include "widget.h"
#include "funwidget.h"
#include "listwidget.h"
#include "texteditfull.h"
#include "splitter.h"
#include "frame.h"
#include "messagethread.h"
#include "messagewidget.h"
#include "itemwidget.h"
#include "messagethread.h"
#include "globalvariables.h"

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
// #include <QtCore/QSignalBlocker>
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
    void mouseMoveEvent(QMouseEvent *event) override;

private:
    void textCopy();
    void messageRenewResponse();
    void checkGraphicsBackend();
    void isItemShowFull(QWidget *widget);

    QQuickWindow *window;

    bool mouseLeftButtonIsPress;
    RegionEnum regionDir;
    int padding;

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
    // TitleWidget     *titleWidget;
    QList<MessageWidget *> messageWidgetList;
    QQueue<QString> messageQueue;
    QList<int> thinkTimeLengthList;
    // ChatRecordsWidget *chatRecordsWidget;
    QString curChatFile;
    QPropertyAnimation *chatRecordsAnimationMove;
    bool chatRecordsWidgetIsOpen;
    // PrintLabel *emptyTextLabel;
    // PrintLabel *textCopyLabel;

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
    int uiRectWidth;
    int uiRectHeight;
    bool isChangeRectFirst;
    QList<QScreen *> screens;
    bool isDpiChanged;
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
    void messageWidgetResize(MessageWidget *selfMessageWidget);
    void sendMessage();
    void onExecuteNext();
    void startThread();
    void messageStart();
    void queueMessage(const QString &text);
    void recvMessage(const QString &text);
    void messageFinish();
    void getSetTexting(bool);
    void showChatRecords();
    void newChat();
};
#endif // MAINWINDOW_H
