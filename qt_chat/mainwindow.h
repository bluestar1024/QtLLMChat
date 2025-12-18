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

#include <QtWidgets/QMainWindow>
#include <QtWidgets/QGraphicsDropShadowEffect>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QListWidgetItem>
#include <QtCore/QTimer>
#include <QtCore/QString>
#include <QtGui/QScreen>
#include <QtWidgets/QApplication>
#include <QtCore/QVariant>

extern const QString imagesDir;
extern const QString fontFilePath;
extern const QString mathjaxScriptPath;
extern const int windowFontPointSize;
extern const int windowFontPixelSize;
extern const int titleFontPixelSize;
extern const int buttonFontPointSize;
extern const QColor textEditFullBGColor;
extern const QColor textEditFullBGTColor;
extern const QColor textEditFullBTColor;
extern const QColor textEditFullBColor;
extern const QString testText;

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

private:
    void textCopy();
    void messageRenewResponse();

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
    QList<int> thinkTimeLengthList;
    // ChatRecordsWidget *chatRecordsWidget;
    QString curChatFile;
    QPropertyAnimation *chatRecordsAnimationMove;
    bool chatRecordsWidgetIsOpen;
    // PrintLabel *emptyTextLabel;
    // PrintLabel *textCopyLabel;

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
    void messageWidgetResize();
    void sendMessage();
    void onExecuteNext();
    void startThread();
    void messageStart();
    void recvMessage(const QString &text);
    void messageFinish();
    void getSetTexting(bool);
    void showChatRecords();
    void newChat();
};
#endif // MAINWINDOW_H
