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
#include "appcontext.h"

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
#include <QtCore/QByteArray>
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
#ifdef Q_OS_WIN
    // 取当前原生窗口句柄：仅当平台窗口已创建完成时返回有效句柄，否则返回
    // nullptr；不做任何创建动作（对比 winId() 会触发窗口创建——在窗口创建
    // 过程中分发的消息处理里调用会造成创建重入、CreateWindowEx 失败）
    HWND nativeHandle() const;
#endif
    // 确保窗口样式包含边缘拉伸所需的位（WS_THICKFRAME 等）：Qt 平台层在部分路径
    // 会按 windowFlags 重算样式整体写回 GWL_STYLE（FramelessWindowHint 窗口不含
    // 这些位），一旦丢失鼠标将无法拖拽窗口边缘；检测缺失时补齐并重算非客户区
    void ensureFrameStyle(const char *reason);
    // 轻量检查窗口样式，缺失时延迟到事件循环空闲补齐：供 WM_NCCALCSIZE /
    // WM_NCHITTEST / WM_ENTERSIZEMOVE / WM_EXITSIZEMOVE 等高频或在拖拽模态
    // 循环内触发的消息处理调用，避免在消息处理过程中同步重写样式
    void scheduleFrameStyleEnsure();
    // 诊断：鼠标在拉伸边缘/非拉伸区之间切换时打印一次当前命中区域与窗口样式
    void logHitTestRegion(const char *region);
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
    // 切换聊天记录/新建聊天的重入保护：generateCurChatRecord 创建 MessageWidget 时会进入
    // 渲染等待的嵌套事件循环，快速连续点击的记录列表事件会在构建中途重入本流程。
    // 重入时只记录最新目标，等当前流程（切换/重建）结束后统一执行，避免半成品列表
    // 被写回文件（记录文件被截断）、新旧会话消息混入同一列表（显示与文件不符）
    void requestChatRecordSwitch(const QString &fileName, bool isNewChat);
    // 真正执行一次会话切换/新建聊天：停止接收线程、保存当前记录、清空列表后重建/置空
    void applyChatRecordSwitch(const QString &fileName, bool isNewChat);
    // 会话切换/新建聊天时停止接收链：清空积压队列、复位流式渲染状态、
    // 断开接收链上指向旧消息控件的裸指针（旧控件随后被 deleteLater/clear 销毁）
    void resetRecvChain();
    // 按聊天记录部件的展开状态计算 chatShow 宽度（不读取 chatShow->width()，
    // 避免展开/收起动画过程中读到的中间宽度影响 MessageWidget 最大宽度）
    int chatShowWidth(bool recordsExpanded) const;
    // MessageWidget 的最大宽度：与展开状态无关，未展开时约为 chatShow 宽度的 2/3，
    // 展开时 AI 消息达到该宽度后右边缘与用户消息 ImageLabel 右边缘对齐
    int messageWidgetMaxWidth() const;
    // 按当前 chatShow 宽度同步单个 item 部件的尺寸、布局边距与 sizeHint
    void updateItemLayout(QWidget *itemWidget, MessageWidget *messageWidget, QListWidgetItem *item);
    // 聊天记录部件展开/收起后只调整 item 布局边距，不重建 MessageWidget
    void messageWidgetItemRelayout();

    // 用户消息 item 的右侧留白：用户消息 ImageLabel 右边缘与 chatShow 右边缘的距离，
    // 同时也是 AI 消息达到最大宽度时右边缘的对齐基准
    static constexpr int itemRightMargin = 25;
    // item 布局的上下留白
    static constexpr int itemVerticalMargin = 5;
    // MessageWidget 最大宽度下限，避免窗口过窄时算出非正值
    static constexpr int minMessageWidgetMaxWidth = 300;

    AppContext *appContext;

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
    // generateCurChatRecord 正在按文件构建消息列表（列表为半成品，禁止序列化写回文件）
    bool isBuildingChatRecord;
    // 上一次构建被连续点击中止（列表只含部分消息，不代表任何文件的完整内容）
    bool isChatRecordBuildAbandoned;
    // 切换聊天记录/新建聊天流程执行中（含被推迟目标的执行阶段）
    bool isSwitchingChatRecord;
    // 构建/重建期间收到、等待执行的切换目标（快速连续点击只保留最后一次）
    bool hasPendingChatSwitch;
    bool pendingChatSwitchIsNewChat;
    QString pendingChatSwitchFile;
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
    bool isMinimizedState;
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
    // 样式补齐任务已排队标记：WM_NCCALCSIZE/WM_NCHITTEST 等高频消息里避免重复入队
    bool styleEnsurePosted = false;
    // 上一次 WM_NCHITTEST 命中区域（仅变化时打印诊断日志，避免高频噪声）
    QByteArray lastHitTestRegion;

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
