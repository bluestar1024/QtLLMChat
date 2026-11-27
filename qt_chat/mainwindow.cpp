#include "mainwindow.h"
#include "dpiutil.h"

#include <cmath>

#include <QtWebEngineCore/QWebEngineProfile>
#include <QtQuickWidgets/QQuickWidget>
#include <QtWidgets/QApplication>
#include <QtGui/QGuiApplication>
#include <QtGui/QFont>
#include <QDebug>
#include <QPointer>

namespace {
// 本窗口内的布局尺寸均以 144 DPI 基准（DpiUtil::kBaseDpi）在代码中给定，
// 其它 DPI 按基准 × dpi / 144 换算：
//   - 窗口初始尺寸与最小拖动尺寸（WM_GETMINMAXINFO）
//   - chatShowWidget / chatInputWidget 的最小高度
//   - chatShowVLayout / chatInputVLayout 的内边距
constexpr int kBaseWindowWidth = 1200;
constexpr int kBaseWindowHeight = 800;
constexpr int kBaseWindowMinWidth = 1100;
constexpr int kBaseWindowMinHeight = 795;
constexpr int kBaseChatShowMinHeight = 244;
constexpr int kBaseChatInputMinHeight = 100;
const QMargins kBaseChatShowMargins(20, 4, 10, 16);
const QMargins kBaseChatInputMargins(20, 0, 20, 20);
// 浮动提示标签（emptyTextLabel / textCopyLabel）与聊天显示区下边缘之间的间距：
// 其位置的其余组成（标题栏 / 功能栏 / 聊天显示区高度）由各部件自身的 DPI 缩放链
// 给出，只有这里是代码中的固定像素，需同样以 144 基准换算
constexpr int kBaseFloatingLabelGap = 10;
} // namespace

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      message(""),
      isShowFirst(true),
      isProcessing(false),
      isRegenerating(false),
      isRegeneratePending(false),
      isRegenerateScheduled(false),
      isBuildingChatRecord(false),
      isChatRecordBuildAbandoned(false),
      isSwitchingChatRecord(false),
      hasPendingChatSwitch(false),
      pendingChatSwitchIsNewChat(false),
      pushButtonIsPress(false),
      screenChanged(false),
      isSending(false),
      isThreadFinished(false),
      isContinueShow(true),
      dragStartWidth(-1),
      dragStartHeight(-1),
      baseWindowSize(QSize(-1, -1)),
      baseWindowDpi(0),
      baseChatInputHeight(-1),
      isSizeMoveDrag(false),
      dragRegenerateDone(false),
      pendingRegenerateAfterResize(false),
      isDpiDragPending(false),
      pendingDragDpi(0),
      dpiDragHandled(false),
      dpiChainScheduleRegenerate(true),
      isMinimizedState(false),
      lastRegenerateWidth(-1),
      lastRegenerateHeight(-1),
      isDpiChanged(false),
      avoidRepeatSelfFun(false)
{
    appContext = new AppContext();
    // ===== 构造期一律以 144 DPI 基准建界面 =====
    // 各部件的默认尺寸、边距、最小高度与像素字号（AppContext 默认 20/28px）都是
    // 144 DPI 屏（150% 缩放）下的数值。窗口初始化完成、实际落到某块屏后，由
    // showEvent 按该屏 DPI 从基准换算一次（见其中 applyScreenDpiChange 调用）：
    // 启动屏不是 144 DPI 时（例如只在 96 DPI 屏上运行），窗口与部件尺寸、最小
    // 尺寸、appliedDpi、像素字号都在首帧绘制前统一换算到该屏，物理尺寸与
    // 144 DPI 屏一致。构造期也不再用“当前屏 DPI”解释这些基准尺寸（否则基准会
    // 被记成 1800x1200 之类的错误值）
    // ===== 应用级默认字体与 ToolTip 样式 =====
    // 应用级默认字体：兜底未显式 setFont 的控件（QToolTip 弹窗、QMessageBox 等）；
    // 字体族经 AppContext::fontFamily() 懒加载注册（进程内单次注册，与各控件
    // 共享注册结果，避免重复注册使 Qt 字体数据库累积）
    const QString &fontFamily = appContext->fontFamily();
    if (!fontFamily.isEmpty())
        QApplication::setFont(QFont(fontFamily, appContext->windowFontPointSize()));
    // 全局样式表仅含 QToolTip 选择器：圆角深色提示样式，不影响其他控件
    qApp->setStyleSheet(QStringLiteral("QToolTip{"
                                       "border: none;"
                                       "border-radius: 10px;"
                                       "color: white;"
                                       "background: #404040;"
                                       "}"));
    // ===== 预置隐藏的 RHI 占位控件（消除首个 WebEngineView 加入时的窗口重建） =====
    // 原生窗口由构造函数末尾的 ensureFrameStyle("constructor") 提前创建，Qt 在创建
    // QWidgetWindow 时会评估部件树中是否存在需要 RHI 合成的控件（QQuickWidget 等）
    // 来决定窗口的 SurfaceType。此前树内没有这类控件，窗口以 RasterSurface 出生；
    // 之后第一个 MessageWidget 内的 WebEngineView（其内部渲染委托是 QQuickWidget，
    // 需要 Direct3DSurface）加入部件树时，Qt 检测到 SurfaceType 不一致，会销毁并
    // 重建原生窗口，导致整个界面消失后再出现。
    // 预先放置一个隐藏的占位 QQuickWidget（1×1、不参与任何布局、不显示），使窗口
    // 从创建起即为正确的 SurfaceType；后续 WebEngineView 加入时只需初始化 RHI
    // 合成通道（createRhi + usesRhiFlush），原生窗口不再被重建。
    // 注意：控件需保持常驻（在窗口创建前销毁会使评估落空），隐藏状态下开销可忽略。
    QQuickWidget *rhiProbeWidget = new QQuickWidget(this);
    rhiProbeWidget->resize(1, 1);
    rhiProbeWidget->hide();
    // ===== WebEngine 引擎预热（不创建额外 page） =====
    // 原 dummyView 通过隐藏 QWebEngineView + about:blank 预热，会额外创建一个 page。
    // 相比这里只初始化 browser 进程，dummyView 还多预热了 GPU 侧（Qt WebEngine 默认
    // --in-process-gpu，即 browser 进程内的 GPU 线程/GL 上下文，属于全局共享资源）；
    // 但原实现从未 show()，loadFinished 后立即 stop() 并延迟删除，合成器不会真正渲染，
    // GPU/首帧初始化未必完成；且 about:blank 与真实页面的 data:/file:// SiteInstance
    // 不同，renderer 进程大概率不会被后续页面复用，删除后又要重新启动。
    // 因此这里不建 page，只提前初始化 WebEngineContext（ContentMainRunner、
    // BrowserTaskExecutor 等），避免额外 WebContents/renderer 的内存与生命周期风险；
    // 若将来实测首条消息仍明显偏慢，应改为“预热 page 复用为第一个真实页面”，
    // 而不是创建后丢弃的 dummyView。
    // 延迟到 singleShot(0) 执行，避免 Chromium 初始化阻塞主窗口首次显示。
    QTimer::singleShot(0, qApp, []() { QWebEngineProfile::defaultProfile(); });

    // 窗口初始尺寸与最小尺寸以 144 DPI 基准常数给出（最小尺寸与 WM_GETMINMAXINFO
    // 使用同一常量）：其它 DPI 屏上由 showEvent 在首次显示前按基准换算
    setMinimumSize(kBaseWindowMinWidth, kBaseWindowMinHeight);
    resize(kBaseWindowWidth, kBaseWindowHeight);
    setWindowFlags(Qt::FramelessWindowHint | Qt::WindowSystemMenuHint);
    setMouseTracking(true);

    // mouseLeftButtonIsPress = false;
    // regionDir = RegionEnum::Middle;
    // padding = 2;
    titleWidgetInit();
    funWidgetInit();
    chatShow = new ListWidget();
    chatShowWidget = new Widget();
    chatShowWidget->resize(1200, 460);
    chatShowWidget->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    chatShowVLayout = new QVBoxLayout(chatShowWidget);
    chatShowVLayout->addWidget(chatShow);
    chatInput = new TextEditFull(appContext);
    chatInput->connectSendButtonClick(this, &MainWindow::sendMessage);
    // 输入框顶部胶囊拖动改高：分隔条交互已禁用，高度调节改由胶囊接管
    connect(chatInput, &TextEditFull::resizeGripPressed, this,
            &MainWindow::chatInputResizeGripPressed);
    connect(chatInput, &TextEditFull::resizeGripMoved, this,
            &MainWindow::chatInputResizeGripMoved);
    connect(chatInput, &TextEditFull::resizeGripReleased, this,
            &MainWindow::chatInputResizeGripReleased);
    chatInputWidget = new Widget();
    chatInputWidget->resize(1200, 240);
    chatInputWidget->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    chatInputVLayout = new QVBoxLayout(chatInputWidget);
    chatInputVLayout->addWidget(chatInput);
    // 最小高度与内边距均为 144 DPI 基准下的默认值，实际值在窗口落到其它 DPI
    // 屏时按基准重新换算（见 applyLayoutDpiScale）
    applyLayoutDpiScale(DpiUtil::kBaseDpi);
    splitter = new Splitter(Qt::Vertical);
    splitter->resize(1200, 700);
    splitter->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    splitter->setChildrenCollapsible(false);
    splitter->addWidget(chatShowWidget);
    splitter->addWidget(chatInputWidget);
    splitter->setContentsMargins(0, 0, 0, 0);
    splitter->setStretchFactor(0, 1);
    splitter->setStretchFactor(1, 0);
    splitter->setHandleWidth(0);
    // 用户拖动分隔条改变 chatInput 高度时回写高度基准（折算回 144 DPI）：
    // 只在用户主动调高时更新，跨屏与窗口缩放时的临时布局不会污染基准
    connect(splitter, &QSplitter::splitterMoved, this, [this]() {
        writeBackChatInputHeightBase();
    });
    contentWidget = new Widget();
    contentWidget->resize(1200, 760);
    contentWidget->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    contentVLayout = new QVBoxLayout(contentWidget);
    contentVLayout->addWidget(chatFun);
    contentVLayout->addWidget(splitter);
    contentVLayout->setContentsMargins(0, 0, 0, 0);
    contentVLayout->setSpacing(0);
    contentVLayout->setStretch(0, 0);
    contentVLayout->setStretch(1, 1);
    mainWidget = new Widget();
    mainWidget->resize(size());
    mainWidget->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    mainWidget->setObjectName("mainWidget");
    mainWidget->setStyleSheet("#mainWidget {"
                              "    background-color: #F0F0F0;"
                              "}");
    setCentralWidget(mainWidget);
    mainVLayout = new QVBoxLayout(mainWidget);
    mainVLayout->addWidget(titleWidget);
    mainVLayout->addWidget(contentWidget);
    mainVLayout->setContentsMargins(0, 0, 0, 0);
    mainVLayout->setSpacing(0);
    mainVLayout->setStretch(0, 0);
    mainVLayout->setStretch(1, 1);
    qDebug() << "FunWidget geometry:" << chatFun->geometry();
    // QGraphicsDropShadowEffect *shadow = new QGraphicsDropShadowEffect(this);
    // shadow->setBlurRadius(20);
    // shadow->setOffset(0, 0);
    // shadow->setColor(QColor(0, 0, 0, 150));
    // setGraphicsEffect(shadow);
    settingWidgetInit();
    chatRecordsWidgetInit();
    emptyTextLabel = new PrintLabel("文本不能为空", appContext, this);
    emptyTextLabel->raise();
    emptyTextLabel->hide();
    textCopyLabel = new PrintLabel("文本复制成功", appContext, this);
    textCopyLabel->raise();
    textCopyLabel->hide();
    // 两个标签的位置同样以 144 DPI 基准给出（构造期所有尺寸都是基准值），
    // 窗口首次显示与每次 DPI 变化时由 updateFloatingLabelPosition 重算
    updateFloatingLabelPosition(DpiUtil::kBaseDpi);

    messageWidgetList.clear();
    // 拖拽中的消息宽度轻量刷新节流（见 scheduleMessageWidthRefresh）：
    // 单次触发 + 80ms 间隔，一次拖拽中连续的 resize 只做一次刷新
    messageWidthRefreshTimer.setSingleShot(true);
    messageWidthRefreshTimer.setInterval(80);
    connect(&messageWidthRefreshTimer, &QTimer::timeout, this,
            &MainWindow::refreshMessageWidthDuringDrag);
    lastScreen = curScreen = screen();
    // 构造期尺寸全部属于 144 DPI 基准（见构造函数开头说明）：curDpi 同样以基准
    // 初始化。窗口首次显示时若实际所在屏 DPI 不同，showEvent 会以 lastDpi=144
    // 执行一次完整换算（appliedDpi、最小尺寸/最小高度、布局边距同步更新）
    initDpi = lastDpi = curDpi = DpiUtil::kBaseDpi;
    // DPI 物理尺寸基准：初始窗口尺寸即 144 DPI 基准尺寸（见 dpiScaledSize）
    baseWindowSize = QSize(kBaseWindowWidth, kBaseWindowHeight);
    baseWindowDpi = DpiUtil::kBaseDpi;
    // chatInput 的高度基准（144 DPI 屏的像素高度）：直接取 TextEditFull 的
    // 144 基准常量，不做 DPI 折算——构造期 mainwindow 与 TextEditFull 各自
    // 通过 screen() 取到的屏可能不同（窗口尚未落到实际屏），折算会把基准算偏
    baseChatInputHeight = TextEditFull::baseHeightPx();
    // 拖拽状态标记初始值（WM_ENTERSIZEMOVE / WM_SIZING / WM_EXITSIZEMOVE 每次
    // 拖拽循环开始时重新置位，这里只为避免未初始化读取）
    dpiDragHandled = false;
    userResizeDuringDrag = false;
    chatInputGripStartY = 0;
    // 提示字体按窗口所在屏的 DPI 换算为像素字号（详见 AppContext::
    // applyToolTipFontForScreen）；按钮显示提示前还会按自身所在屏再校正
    applyToolTipFont();
    thinkExpandedList.clear();
    thinkTimeLengthList.clear();
    // lastNormalGeometry = geometry();
    // uiRectWidth = width();
    // uiRectHeight = height();
    // 屏对象由 Qt 随显示拓扑变化动态创建/销毁：缓存列表与 DPI 信号连接需在
    // 拓扑变化时重建（详见 refreshScreenList）
    refreshScreenList();
    if (QGuiApplication *guiApp =
                qobject_cast<QGuiApplication *>(QCoreApplication::instance())) {
        connect(guiApp, &QGuiApplication::screenAdded, this,
                [this](QScreen *) { refreshScreenList(); });
        connect(guiApp, &QGuiApplication::screenRemoved, this,
                [this](QScreen *) { refreshScreenList(); });
        connect(guiApp, &QGuiApplication::primaryScreenChanged, this,
                [this](QScreen *) { refreshScreenList(); });
    }

    widgetSizeDict["MainWindow"] = DpiUtil::toBaseSize(size(), curDpi);
    // 最小尺寸直接以基准值记录：构造时按基准像素设置，不按构造屏 DPI 折算
    // （窗口最终落在哪块屏由 showEvent 校正）
    widgetSizeDict["MainWindow minimumSize"] = QSize(kBaseWindowMinWidth, kBaseWindowMinHeight);
    widgetSizeDict["mainWidget"] = DpiUtil::toBaseSize(mainWidget->size(), curDpi);
    widgetSizeDict["mainWidget x"] = DpiUtil::toBasePx(mainWidget->x(), curDpi);
    widgetSizeDict["mainWidget y"] = DpiUtil::toBasePx(mainWidget->y(), curDpi);

    // checkGraphicsBackend();

#ifdef Q_OS_WIN
    // 窗口创建后补齐边缘拉伸所需的样式位（Qt 按 windowFlags 生成的样式不含
    // WS_THICKFRAME）；运行期由 ensureFrameStyle/scheduleFrameStyleEnsure 兜底
    ensureFrameStyle("constructor");

    // HWND hwnd = reinterpret_cast<HWND>(winId());
    // DWORD style = GetWindowLong(hwnd, GWL_STYLE);
    // style = (style & ~WS_CAPTION) | WS_THICKFRAME | WS_MAXIMIZEBOX | WS_MINIMIZEBOX;
    // SetWindowLongPtr(hwnd, GWL_STYLE, style);

    // DWORD exStyle = GetWindowLong(hwnd, GWL_EXSTYLE);
    // exStyle |= WS_EX_LAYERED;
    // SetWindowLongPtr(hwnd, GWL_EXSTYLE, exStyle);

    // SetWindowPos(hwnd, NULL, 0, 0, 0, 0,
    //              SWP_FRAMECHANGED | SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOOWNERZORDER);
#endif
}

MainWindow::~MainWindow()
{
    delete chatRecordStore;
    chatRecordStore = nullptr;
}

void MainWindow::refreshScreenList()
{
    // 显示器断电/休眠重连、插拔、RDP 会话切换、修改缩放比例引起的显示拓扑重算，
    // 都会让 Qt 销毁并重建 QScreen 对象。若只在构造时取一次 QApplication::screens()，
    // 屏被移除后列表里就留下悬空 QScreen*：拖拽窗口时 WM_MOVING 遍历它读取
    // geometry()，即访问已销毁对象（Qt6Gui 内 QScreen::geometry() 访问 this->d_ptr
    // 崩溃）。
    // 拓扑变化时重建列表，并重新连接各屏的 DPI 变化信号（新屏对象没有旧连接）
    const QList<QScreen *> currentScreens = QGuiApplication::screens();
    for (QScreen *s : currentScreens) {
        if (!s)
            continue;
        connect(s, &QScreen::logicalDotsPerInchChanged, this, &MainWindow::onDpiChanged,
                Qt::UniqueConnection);
    }
    screens = currentScreens;
    // 缓存的“窗口所在屏”同样可能失效：统一以 Qt 维护的当前屏重新取值
    lastScreen = curScreen = screen();
}

void MainWindow::titleWidgetInit()
{
    titleWidget = new TitleWidget(appContext);
    connect(titleWidget, &TitleWidget::minimizeClicked, this, &MainWindow::uiMinimize);
    connect(titleWidget, &TitleWidget::maximizeClicked, this, &MainWindow::uiMaximize);
    connect(titleWidget, &TitleWidget::closeClicked, this, &MainWindow::uiClose);
}

void MainWindow::funWidgetInit()
{
    chatFun = new FunWidget(appContext);
    // qDebug() << "FunWidget size:" << chatFun->size();
    chatFun->connectChatRecordsButtonClick(this, &MainWindow::showChatRecords);
    chatFun->connectNewChatButtonClick(this, &MainWindow::newChat);
}

void MainWindow::settingWidgetInit()
{
    settingWidget = new SettingWidget(appContext, mainWidget);
    settingWidget->setGeometry(-mainWidget->width() / 3, titleWidget->height(),
                               mainWidget->width() / 3,
                               mainWidget->height() - titleWidget->height());

    settingAnimationMove = new QPropertyAnimation(settingWidget, "geometry");
    settingAnimationMove->setDuration(1000);
    settingAnimationMove->setEasingCurve(QEasingCurve::OutQuad);

    settingWidgetIsOpen = false;
}

void MainWindow::chatRecordsWidgetInit()
{
    chatRecordsWidget = new ChatRecordsWidget(appContext, mainWidget);
    chatRecordsWidget->setGeometry(-mainWidget->width() / 3, titleWidget->height(),
                                   mainWidget->width() / 3,
                                   mainWidget->height() - titleWidget->height());
    chatRecordsWidget->connectSettingButtonClick(this, &MainWindow::settingButtonClicked);
    chatRecordsWidget->connectLineEditTextChanged(this, &MainWindow::showSearchRecords);
    chatRecordsWidget->connectSearchButtonClick(this, &MainWindow::showSearchRecords);
    chatRecordsWidget->connectClearAllButtonClick(this, &MainWindow::clearAllChatRecords);
    chatRecordsWidget->connectListItemClick(this, &MainWindow::generateChatRecord);

    chatRecordsAnimationMove = new QPropertyAnimation(chatRecordsWidget, "geometry");
    chatRecordsAnimationMove->setDuration(1000);
    chatRecordsAnimationMove->setEasingCurve(QEasingCurve::OutQuad);
    connect(chatRecordsAnimationMove, &QPropertyAnimation::valueChanged, this,
            &MainWindow::chatRecordsUiAnimationMove);
    connect(chatRecordsAnimationMove, &QPropertyAnimation::finished, this,
            &MainWindow::chatRecordsUiMoveFinished);

    curChatSessionId = -1;
    // 聊天记录数据库：与聊天记录同目录（chatrecords/chat_records.db）
    chatRecordStore =
            new ChatRecordStore(QDir(appContext->chatRecordsDir()).filePath("chat_records.db"));
    chatRecordsWidgetIsOpen = false;
}

// void MainWindow::checkGraphicsBackend()
// {
//     qwindow = new QQuickWindow();
//     qwindow->show();
//     QTimer::singleShot(500, [&]() {
//         qDebug() << "===== 图形后端信息 =====";
//         qDebug() << "当前渲染器:" << qwindow->rendererInterface()->graphicsApi();
//         qDebug() << "场景图后端:" << qwindow->sceneGraphBackend();
//         qDebug() << "======================";
//         qwindow->close();
//         qwindow->deleteLater();
//     });
// }

void MainWindow::applyDWMShadow()
{
#ifdef Q_OS_WIN
    HWND hwnd = reinterpret_cast<HWND>(winId());
    if (!hwnd || !IsWindow(hwnd)) {
        return;
    }

    DWMNCRENDERINGPOLICY policy = DWMNCRP_ENABLED;
    DwmSetWindowAttribute(hwnd, DWMWA_NCRENDERING_POLICY, &policy, sizeof(policy));

    // Win11 会为窗口绘制 1px 边框（DWMWA_BORDER_COLOR），无边框窗口贴边时这条
    // 边框会画在客户区边缘上，使标题栏与屏幕边缘错开 1px；DWMWA_COLOR_NONE
    // 关闭该边框（旧系统无此属性时调用失败，忽略即可）
    const COLORREF borderNone = 0xFFFFFFFE; // DWMWA_COLOR_NONE
    DwmSetWindowAttribute(hwnd, 34 /* DWMWA_BORDER_COLOR */, &borderNone, sizeof(borderNone));

    MARGINS margins = { 1, 1, 1, 1 };
    DwmExtendFrameIntoClientArea(hwnd, &margins);

    RedrawWindow(hwnd, NULL, NULL, RDW_FRAME | RDW_INVALIDATE | RDW_UPDATENOW);
    // SetWindowPos(hwnd, NULL, 0, 0, 0, 0,
    //              SWP_FRAMECHANGED | SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOOWNERZORDER
    //                      | SWP_NOACTIVATE | SWP_SHOWWINDOW);
#endif
}

void MainWindow::applyToolTipFont()
{
    // 提示字体按窗口实际所在屏的 DPI 换算为像素字号：构造时 screen() 可能
    // 仍是主屏，而窗口实际显示在另一块 DPI 不同的屏上，沿用主屏字号会偏大
    appContext->applyToolTipFontForScreen(screen());
}

#ifdef Q_OS_WIN
HWND MainWindow::nativeHandle() const
{
    // windowHandle()/QWindow::handle() 均为纯读取接口，不会触发窗口创建，
    // 可在窗口创建过程中的消息处理里安全调用；handle() 非空说明平台窗口
    // （QWindowsWindow）已完整创建，其 winId() 为纯 getter 不会再创建窗口
    if (QWindow *qwin = windowHandle()) {
        if (qwin->handle()) {
            return reinterpret_cast<HWND>(qwin->winId());
        }
    }
    return nullptr;
}
#endif

void MainWindow::ensureFrameStyle(const char *reason)
{
#ifdef Q_OS_WIN
    // 优先取已创建的平台窗口句柄（不触发创建）；仅当原生窗口尚不存在
    // （构造函数首次调用）时才通过 winId() 创建原生窗口
    HWND hwnd = nativeHandle();
    if (!hwnd) {
        hwnd = reinterpret_cast<HWND>(winId());
    }
    if (!hwnd || !IsWindow(hwnd)) {
        return;
    }

    // 窗口边缘拖拽拉伸依赖 WS_THICKFRAME：Windows 只对含该样式位的窗口执行
    // WM_NCHITTEST 返回 HTxxx 后的缩放；WS_CAPTION 等位同时支撑贴靠布局。
    // Qt 平台层在部分路径会按 windowFlags 重算样式并整体写回 GWL_STYLE
    // （FramelessWindowHint 窗口不含这些位），丢失后拖拽窗口边缘将完全失效，
    // 在此检测并补齐，使样式在任意路径下都能自愈
    constexpr DWORD needStyle = WS_OVERLAPPEDWINDOW;
    const DWORD style = static_cast<DWORD>(GetWindowLongPtrW(hwnd, GWL_STYLE));
    if ((style & needStyle) == needStyle) {
        return;
    }

    const DWORD fixedStyle = style | needStyle;
    qDebug().nospace() << "ensureFrameStyle(" << reason << ") style: 0x" << Qt::hex << style
                       << " -> 0x" << fixedStyle;
    SetWindowLongPtrW(hwnd, GWL_STYLE, fixedStyle);
    SetWindowPos(hwnd, nullptr, 0, 0, 0, 0,
                 SWP_FRAMECHANGED | SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOOWNERZORDER
                         | SWP_NOACTIVATE);
#else
    Q_UNUSED(reason);
#endif
}

void MainWindow::scheduleFrameStyleEnsure()
{
#ifdef Q_OS_WIN
    if (styleEnsurePosted) {
        return;
    }

    // 只处理已创建的原生窗口：本函数会在窗口创建过程中（WM_NCCALCSIZE 等在
    // CreateWindowEx 内分发）被调用，此处绝不能调用 winId()——它会触发窗口
    // 创建流程重入，导致 CreateWindowEx 失败、主窗口创建崩溃
    HWND hwnd = nativeHandle();
    if (!hwnd || !IsWindow(hwnd)) {
        return; // 原生窗口尚未创建：由构造函数末尾的 ensureFrameStyle 兜底
    }

    constexpr DWORD needStyle = WS_OVERLAPPEDWINDOW;
    if ((static_cast<DWORD>(GetWindowLongPtrW(hwnd, GWL_STYLE)) & needStyle) == needStyle) {
        return;
    }

    // 不在消息处理/拖拽模态循环内同步重写样式：排队到事件循环空闲时补齐
    styleEnsurePosted = true;
    QTimer::singleShot(0, this, [this]() {
        styleEnsurePosted = false;
        ensureFrameStyle("scheduled");
    });
#endif
}

// 递归激活部件及其全部子部件的布局：布局的激活通常只是排队（LayoutRequest 事件），
// 窗口首次映射时若已绘制，画面仍会是排队前的旧几何。首次显示前按本机 DPI 重排
// 全部部件后，需要在返回事件循环前同步激活，保证第一帧即为新布局（见 showEvent）
static void activateLayoutsRecursively(QWidget *widget)
{
    if (!widget)
        return;
    QLayout *layout = widget->layout();
    if (layout) {
        layout->activate();
        // 激活只设置本部件管理的子部件几何，子部件自己的布局仍需继续激活
        const int count = layout->count();
        for (int i = 0; i < count; ++i) {
            QLayoutItem *item = layout->itemAt(i);
            if (item && item->widget())
                activateLayoutsRecursively(item->widget());
        }
    }
    const QList<QWidget *> children = widget->findChildren<QWidget *>(Qt::FindDirectChildrenOnly);
    for (QWidget *child : children) {
        // 已由布局管理的子部件在上方递归过，这里只处理无布局直接锚定/手动定位的
        if (layout && layout->indexOf(child) >= 0)
            continue;
        activateLayoutsRecursively(child);
    }
}

void MainWindow::logHitTestRegion(const char *region)
{
    if (lastHitTestRegion == region) {
        return;
    }
    lastHitTestRegion = region;

#ifdef Q_OS_WIN
    // 用 nativeHandle() 而非 winId()：WM_NCHITTEST 可能在窗口创建过程中被
    // 分发，winId() 会触发创建重入（详见 scheduleFrameStyleEnsure 注释）
    HWND hwnd = nativeHandle();
    qDebug().nospace() << "WM_NCHITTEST region: " << region << " style: 0x" << Qt::hex
                       << (hwnd ? static_cast<DWORD>(GetWindowLongPtrW(hwnd, GWL_STYLE)) : 0);
#else
    qDebug() << "WM_NCHITTEST region:" << region;
#endif
}

void MainWindow::showEvent(QShowEvent *event)
{
    QMainWindow::showEvent(event);
    // 首次显示完成后，WM_SIZE 才允许触发聊天记录重建（避免启动阶段误触发）
    isShowFirst = false;

    // 构造期全部部件以 144 DPI 基准建好（见构造函数开头说明）：窗口首次显示时
    // 按实际所在屏 DPI 换算一次。启动屏就是 144 DPI 时这一步为空操作；启动屏是
    // 其它 DPI（例如 96）时，窗口尺寸与最小尺寸、两个分栏容器的最小高度/布局
    // 边距、全部部件的缓存尺寸、appliedDpi 与像素字号都在首帧绘制前换算完成
    const QScreen *showScreen = screen();
    const int showScreenDpi = showScreen ? qRound(showScreen->logicalDotsPerInch()) : 0;
    if (showScreenDpi > 0 && showScreenDpi != qRound(curDpi)) {
        applyScreenDpiChange(showScreenDpi);
        // 缩放链已将各部件按新屏 DPI 重排（几何/字号已正确），但布局激活
        // 是排队执行的：首次映射时的第一帧若在激活前绘制，画面仍是构造时（144
        // DPI）的旧布局——标题行高 40 而非 27、FunWidget 内部边距 10 而非 7，
        // 使功能区行与图标偏低（用户看到“FunWidget 位置比预期低”）；其后的
        // 重绘又被启动阶段 Chromium 预热阻塞约 0.6-1s（用户感知约 1 秒），
        // 才“跳回”正确位置。这里在返回事件循环前递归激活全部布局并标脏重绘，
        // 保证首帧即为缩放后的布局
        activateLayoutsRecursively(this);
        mainWidget->update();
    }
    // 首次显示后按 144 DPI 基准重设 chatInput 高度：splitter 默认按子项当前
    // 尺寸分配 pane 高度，会把输入框压得比 144 DPI 基准矮；布局激活可能排在
    // 事件循环里，再兜底一次（按基准绝对换算，重复执行结果不变）
    applyChatInputHeightForDpi();
    QTimer::singleShot(0, this, &MainWindow::applyChatInputHeightForDpi);
    // 监听平台窗口的屏幕变化：系统缩放比例变更时窗口会在拓扑重算中临时跨屏
    // 再回到原屏，期间按临时屏 DPI 执行过缩放链，回到原屏后没有可靠的
    // WM_DPICHANGED 时序，界面会停留在临时屏的字号/尺寸上（整变大）；
    // 由此信号在屏切换稳定后自动按实际所在屏 DPI 校正（见 onScreenChanged）
    if (QWindow *win = windowHandle()) {
        connect(win, &QWindow::screenChanged, this, &MainWindow::onScreenChanged,
                Qt::UniqueConnection);
    }

#ifdef Q_OS_WIN
    // static bool firstShow = true;
    // if (firstShow) {
    //     firstShow = false;

    //     HWND hwnd = reinterpret_cast<HWND>(winId());
    //     if (!hwnd)
    //         return;

    //     DWORD style = GetWindowLong(hwnd, GWL_STYLE);

    //     style |= WS_THICKFRAME | WS_MAXIMIZEBOX | WS_MINIMIZEBOX | WS_CAPTION;
    //     style &= ~(WS_BORDER | WS_DLGFRAME);
    //     SetWindowLongPtr(hwnd, GWL_STYLE, style);

    //     DWORD exStyle = GetWindowLong(hwnd, GWL_EXSTYLE);
    //     exStyle &= ~(WS_EX_CLIENTEDGE | WS_EX_WINDOWEDGE | WS_EX_STATICEDGE | WS_EX_LAYERED);
    //     exStyle |= WS_EX_NOREDIRECTIONBITMAP;
    //     SetWindowLongPtr(hwnd, GWL_EXSTYLE, exStyle);

    //     SetWindowPos(hwnd, NULL, 0, 0, 0, 0,
    //                  SWP_FRAMECHANGED | SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOOWNERZORDER
    //                          | SWP_NOACTIVATE);

    //     applyDWMShadow();
    // }
    applyDWMShadow();
    // 首次显示后确认窗口样式：显示路径上 Qt 可能重算样式丢失边缘拉伸位
    ensureFrameStyle("showEvent");
#endif
}

void MainWindow::changeEvent(QEvent *event)
{
#ifdef Q_OS_WIN
    if (event->type() == QEvent::WindowStateChange) {
        QWindowStateChangeEvent *stateEvent = static_cast<QWindowStateChangeEvent *>(event);
        Qt::WindowStates oldState = stateEvent->oldState();
        Qt::WindowStates newState = windowState();

        if ((oldState & Qt::WindowMinimized) && !(newState & Qt::WindowMinimized)) {
            qDebug() << "从最小化恢复";
            // 兜底清除最小化标记：正常路径下 WM_SIZE(SIZE_RESTORED) 已消费该标记
            isMinimizedState = false;
            QTimer::singleShot(10, this, [this]() { applyDWMShadow(); });
        } else if (!(oldState & Qt::WindowMinimized) && (newState & Qt::WindowMinimized)) {
            // 兜底记录最小化状态：恢复时的 WM_SIZE 据此跳过重建
            isMinimizedState = true;
        }
        // if ((newState & Qt::WindowMaximized) && !(oldState & Qt::WindowMaximized)) {
        if (!(oldState & Qt::WindowMaximized) && (newState & Qt::WindowMaximized)) {
            qDebug() << "最大化";
            titleWidget->maxButtonToggleIcon(false);
            QTimer::singleShot(10, this, [this]() { applyDWMShadow(); });
            // } else if ((newState & Qt::WindowNoState) && (oldState & Qt::WindowMaximized)) {
        } else if ((oldState & Qt::WindowMaximized) && !(newState & Qt::WindowMaximized)) {
            qDebug() << "从最大化恢复正常";
            titleWidget->maxButtonToggleIcon(true);
            QTimer::singleShot(10, this, [this]() { applyDWMShadow(); });
        }
        // 状态切换（最大化/还原/最小化/恢复）后确认样式：Qt 状态切换路径
        // 可能重写 GWL_STYLE 丢失边缘拉伸位
        ensureFrameStyle("windowStateChange");
    }
#endif

    QMainWindow::changeEvent(event);
}

bool MainWindow::nativeEvent(const QByteArray &eventType, void *message, qintptr *result)
{
#ifdef Q_OS_WIN
    if (eventType != "windows_generic_MSG")
        return false;

    MSG *msg = static_cast<MSG *>(message);
    HWND hwnd = msg->hwnd;

    switch (msg->message) {
    case WM_NCCALCSIZE: {
        // 窗口尺寸/位置变化都会经过这里：顺带检查样式，缺失时延迟补齐
        // （不在 NcCalcSize 处理中同步重写样式，避免重入）
        scheduleFrameStyleEnsure();
        // 最大化时系统把 SetWindowPos 的矩形当作客户区，并把窗口矩形按其可调整
        // 边框宽度向外膨胀（本机 144 DPI 每边 11px、96 DPI 每边 8px，与
        // AdjustWindowRectExForDpi 返回的边框值一致；Qt 在系统应用完
        // WM_GETMINMAXINFO 的最大化矩形后，还会按屏幕可用区域再 SetWindowPos
        // 一次）。本窗口一般让客户区等于整个窗口矩形（下方返回 0），但最大化时
        // 若也这么做，膨胀会原样成为标题栏与屏幕工作区的偏移。
        // 这里把最大化状态下的客户区钳制为工作区矩形：标题栏左上角与工作区
        // 左上角（右半屏时为顶边中点）严格重合，且不依赖对膨胀量的预测
        if (IsZoomed(hwnd) && msg->wParam) {
            NCCALCSIZE_PARAMS *params = reinterpret_cast<NCCALCSIZE_PARAMS *>(msg->lParam);
            MONITORINFO mi = { sizeof(mi) };
            if (params
                    && GetMonitorInfo(MonitorFromWindow(hwnd, MONITOR_DEFAULTTONEAREST), &mi)) {
                // rgrc[0] 为建议的窗口/客户区矩形（屏幕坐标）：与工作区取交集
                RECT &r = params->rgrc[0];
                r.left = qMax(r.left, mi.rcWork.left);
                r.top = qMax(r.top, mi.rcWork.top);
                r.right = qMin(r.right, mi.rcWork.right);
                r.bottom = qMin(r.bottom, mi.rcWork.bottom);
            }
        }
        *result = 0;
        return true;
    }

    case WM_NCHITTEST: {
        // 命中测试期间顺带检查样式：Qt 平台层可能已把 GWL_STYLE 重写为不含
        // WS_THICKFRAME 的版本，缺失时延迟补齐，恢复鼠标拖拽边缘拉伸的能力
        scheduleFrameStyleEnsure();

        POINT pt;
        pt.x = GET_X_LPARAM(msg->lParam);
        pt.y = GET_Y_LPARAM(msg->lParam);

        ScreenToClient(hwnd, &pt);

        RECT rcClient;
        GetClientRect(hwnd, &rcClient);

        int nX = pt.x;
        int nY = pt.y;
        int w = rcClient.right;
        int h = rcClient.bottom;

        // 统一收敛命中返回值：鼠标在拉伸边缘/非拉伸区之间切换时打印一次
        // 命中的区域与当前窗口样式，便于诊断边缘拖拽问题
        auto hitTestReturn = [&](LRESULT hit, const char *region) {
            logHitTestRegion(region);
            *result = hit;
            return true;
        };

        WINDOWPLACEMENT wp = { sizeof(wp) };
        bool isMaximized = GetWindowPlacement(hwnd, &wp) && wp.showCmd == SW_MAXIMIZE;

        if (isMaximized) {
            int titleHeight = titleWidget->height();

            if (nY >= 0 && nY < titleHeight) {
                int closeBtnWidth = titleWidget->getCloseButtonSize().width();
                int maxBtnWidth = titleWidget->getMaxButtonSize().width();
                int minBtnWidth = titleWidget->getMinButtonSize().width();
                int btnRight = w;
                int btnLeft = btnRight - closeBtnWidth - maxBtnWidth - minBtnWidth;

                if (btnLeft < 0)
                    btnLeft = 0;

                if (nX >= btnLeft && nX < btnRight) {
                    return hitTestReturn(HTCLIENT, "CLIENT(maximized button)");
                }
                return hitTestReturn(HTCAPTION, "CAPTION(maximized)");
            }
            return hitTestReturn(HTCLIENT, "CLIENT(maximized)");
        }

        const int detectBorder = 8;
        const int titleHeight = titleWidget->height();

        int closeBtnWidth = titleWidget->getCloseButtonSize().width();
        int maxBtnWidth = titleWidget->getMaxButtonSize().width();
        int minBtnWidth = titleWidget->getMinButtonSize().width();
        int btnRight = w;
        int btnLeft = btnRight - closeBtnWidth - maxBtnWidth - minBtnWidth;
        if (btnLeft < detectBorder)
            btnLeft = detectBorder;

        if (nX >= 0 && nX < detectBorder && nY >= 0 && nY < detectBorder) {
            return hitTestReturn(HTTOPLEFT, "HTTOPLEFT");
        }
        if (nX >= w - detectBorder && nX < w && nY >= 0 && nY < detectBorder) {
            return hitTestReturn(HTTOPRIGHT, "HTTOPRIGHT");
        }
        if (nX >= 0 && nX < detectBorder && nY >= h - detectBorder && nY < h) {
            return hitTestReturn(HTBOTTOMLEFT, "HTBOTTOMLEFT");
        }
        if (nX >= w - detectBorder && nX < w && nY >= h - detectBorder && nY < h) {
            return hitTestReturn(HTBOTTOMRIGHT, "HTBOTTOMRIGHT");
        }

        if (nY >= 0 && nY < detectBorder && nX >= detectBorder && nX < w - detectBorder) {
            return hitTestReturn(HTTOP, "HTTOP");
        }
        if (nY >= h - detectBorder && nY < h && nX >= detectBorder && nX < w - detectBorder) {
            return hitTestReturn(HTBOTTOM, "HTBOTTOM");
        }
        if (nX >= 0 && nX < detectBorder && nY >= detectBorder && nY < h - detectBorder) {
            return hitTestReturn(HTLEFT, "HTLEFT");
        }
        if (nX >= w - detectBorder && nX < w && nY >= detectBorder && nY < h - detectBorder) {
            return hitTestReturn(HTRIGHT, "HTRIGHT");
        }

        if (nY >= detectBorder && nY < titleHeight) {
            if (nX >= btnLeft && nX < btnRight) {
                return hitTestReturn(HTCLIENT, "CLIENT(button)");
            }
            if (nX >= detectBorder && nX < btnLeft) {
                return hitTestReturn(HTCAPTION, "CAPTION(title)");
            }
        }

        return hitTestReturn(HTCLIENT, "CLIENT");
    }

    case WM_GETMINMAXINFO: {
        MINMAXINFO *mmi = reinterpret_cast<MINMAXINFO *>(msg->lParam);
        MONITORINFO mi = { sizeof(mi) };
        if (GetMonitorInfo(MonitorFromWindow(hwnd, MONITOR_DEFAULTTONEAREST), &mi)) {
            // ptMaxPosition 是“相对目标显示器左上角的偏移量”，不是虚拟屏幕的
            // 绝对坐标：对原点不为 (0,0) 的显示器（本机副屏 x 从 -3440 开始），
            // 直接传 rcWork.left 会被系统再叠加一次显示器原点，最大化后窗口落在
            // 所有显示器之外（实测副屏 maxPos=-3439 得到 x=-6879，3440x1392 的
            // 窗口整体在副屏左侧屏幕外）。这里换算为相对显示器原点的偏移，主屏
            // （原点 0）与副屏（负坐标）都成立
            // 注意：这里的值并不是最大化后的最终矩形——系统应用本结果后，Qt
            // 还会按屏幕可用区域再做一次 SetWindowPos，那一步还会按可调整边框
            // 向外膨胀窗口矩形；最终对齐由 WM_NCCALCSIZE 处理（详见该处）
            const int maxW = mi.rcWork.right - mi.rcWork.left;
            const int maxH = mi.rcWork.bottom - mi.rcWork.top;
            const int relX = mi.rcWork.left - mi.rcMonitor.left;
            const int relY = mi.rcWork.top - mi.rcMonitor.top;
            mmi->ptMaxPosition.x = relX;
            mmi->ptMaxPosition.y = relY;
            mmi->ptMaxSize.x = maxW;
            mmi->ptMaxSize.y = maxH;
        }
        mmi->ptMinTrackSize.x = DpiUtil::scalePx(kBaseWindowMinWidth, int(GetDpiForWindow(hwnd)));
        mmi->ptMinTrackSize.y = DpiUtil::scalePx(kBaseWindowMinHeight, int(GetDpiForWindow(hwnd)));
        *result = 0;
        return true;
    }

    case WM_SIZE: {
        // 尺寸变化（拉伸/最大化/Snap/程序化 resize 等）时顺带检查样式：
        // 缺失时延迟补齐（拖拽期间样式必然完好，此处仅只读检查）
        scheduleFrameStyleEnsure();
        // 最小化：窗口会被移到屏幕外并缩成图标尺寸，该尺寸不代表恢复后的界面尺寸。
        // 仅记录状态供恢复时识别，避免最小化/恢复被当成窗口尺寸变化而触发重建
        if (msg->wParam == SIZE_MINIMIZED) {
            isMinimizedState = true;
            break;
        }
        if (msg->wParam == SIZE_RESTORED || msg->wParam == SIZE_MAXIMIZED) {
            QTimer::singleShot(10, this, &MainWindow::applyDWMShadow);

            RECT rect;
            GetWindowRect(hwnd, &rect);
            int w = rect.right - rect.left;
            int h = rect.bottom - rect.top;

            // 从最小化恢复：窗口恢复到最小化前的尺寸（lastRegenerateWidth 即该尺寸），
            // 直接按最小化前的界面继续显示，不重建 MessageWidget。
            // 最小化前若为最大化，恢复时后续还会收到 SIZE_MAXIMIZED，仍走下方宽度比较
            if (isMinimizedState && msg->wParam == SIZE_RESTORED) {
                isMinimizedState = false;
                lastRegenerateHeight = h;
                qDebug() << "WM_SIZE restore from minimized, skip regenerate" << w << h;
                break;
            }
            isMinimizedState = false;

            // 首次显示阶段（showEvent 之前）的 WM_SIZE：只记录初始尺寸作为重建基准。
            // 否则 lastRegenerateWidth 始终为初值，此后任何一次 WM_SIZE（如从任务栏
            // 恢复窗口、DWM 触发的尺寸通知）都会被误判为宽度变化并重建 MessageWidget
            if (isShowFirst) {
                lastRegenerateWidth = w;
                lastRegenerateHeight = h;
                break;
            }

            // 双击标题栏/单击最大化按钮等非拖拽方式的最大化/还原：
            // 不经过 WM_ENTERSIZEMOVE/WM_EXITSIZEMOVE，只能在这里触发重建；
            // 与上次重建尺寸相同（如 Snap 后延迟到达的 WM_SIZE）则跳过
            if (!isSizeMoveDrag) {
                // 仅宽度变化才重建：MessageWidget 的宽度只取决于窗口宽度，
                // 只改变高度（上下拉伸）不影响文本换行，重建只带来无谓开销与闪烁
                if (w != lastRegenerateWidth) {
                    lastRegenerateWidth = w;
                    lastRegenerateHeight = h;
                    qDebug() << "WM_SIZE isRegenerate" << w << h;
                    // 延迟到 Qt resizeEvent 执行后再重建，保证 chatShow 宽度已更新
                    scheduleMessageWidgetRegenerate();
                } else {
                    lastRegenerateHeight = h;
                    qDebug() << "WM_SIZE skip regenerate, width unchanged" << w << h;
                }
            }
        }
        break;
    }

    case WM_ENTERSIZEMOVE: {
        // 拖动标题栏移动或调整大小开始：记录窗口尺寸，供 WM_MOVING/WM_EXITSIZEMOVE
        // 区分“仅移动”与“调整大小”（仅移动不应触发聊天记录重建）；同时记录
        // DPI 物理尺寸基准（详见 baseWindowSize）
        isSizeMoveDrag = true;
        dragRegenerateDone = false;
        isDpiDragPending = false;
        pendingDragDpi = 0;
        dpiDragHandled = false;
        userResizeDuringDrag = false;
        // 用户开始拖动/拉伸窗口：只在窗口尺寸与其所在屏 DPI 一致（或基准尚未
        // 建立）时，才把当前尺寸作为 DPI 物理尺寸基准。系统在缩放设置变更的
        // 拓扑重算中会临时把窗口改成与基准不成比例的尺寸（过渡帧），最大化/
        // 贴边 Snap 状态下窗口尺寸也由系统布局给出——若把这些尺寸当基准，窗口
        // 基准就与部件基准（widgetSizeDict）不一致：mainWidget 按旧基准铺满后
        // 超出窗口（实测过渡帧 1100x795@144 被当基准后拖到 96 DPI 副屏，窗口
        // 得到 733x530 而 mainWidget 为 800x533），来回跨屏还会逐次漂移。
        // 用户真正拉伸的结果由 WM_EXITSIZEMOVE 采用
        const QSize expectWindowSize = dpiScaledSize(curDpi);
        if (!expectWindowSize.isValid() || !(baseWindowDpi > 0)
                || (qAbs(width() - expectWindowSize.width()) <= 2
                    && qAbs(height() - expectWindowSize.height()) <= 2)) {
            baseWindowSize = size();
            baseWindowDpi = curDpi;
        }
        // 拖拽开始前顺带检查样式（缺失时延迟补齐，恢复后续边缘拉伸能力）
        scheduleFrameStyleEnsure();
        RECT rect;
        GetWindowRect(hwnd, &rect);
        dragStartWidth = rect.right - rect.left;
        dragStartHeight = rect.bottom - rect.top;
        break;
    }

    case WM_MOVING: {
        // 跨 DPI 屏拖动检测（须在 dragRegenerateDone 提前 break 之前执行）：
        // 用拖动目标矩形中心所在屏的 DPI 判断本次拖动是否跨 DPI 屏。系统在
        // 拖动中按 DPI 缩放窗口（WM_DPICHANGED）并触发 resizeEvent，resizeEvent
        // 据此提前执行完整 DPI 缩放链；若等释放（WM_EXITSIZEMOVE）再缩放，
        // 普通布局与 DPI 链会先后各缩放一次子部件，物理尺寸变化过大（Bug）
        {
            RECT *mrc = reinterpret_cast<RECT *>(msg->lParam);
            const POINT center = { (mrc->left + mrc->right) / 2,
                                   (mrc->top + mrc->bottom) / 2 };
            const HMONITOR mon = MonitorFromPoint(center, MONITOR_DEFAULTTONEAREST);
            MONITORINFO mi = { sizeof(mi) };
            int targetDpi = 0;
            if (mon && GetMonitorInfoW(mon, &mi)) {
                const QRect monRect(mi.rcMonitor.left, mi.rcMonitor.top,
                                    mi.rcMonitor.right - mi.rcMonitor.left,
                                    mi.rcMonitor.bottom - mi.rcMonitor.top);
                // 取实时屏列表而不是成员缓存：即使拓扑变化信号尚未处理到，
                // 也不会遍历到已被 Qt 销毁的 QScreen（见 refreshScreenList）
                const QList<QScreen *> currentScreens = QGuiApplication::screens();
                for (const QScreen *s : currentScreens) {
                    if (!s)
                        continue;
                    if (s->geometry() == monRect) {
                        targetDpi = qRound(s->logicalDotsPerInch());
                        break;
                    }
                }
            }
            if (targetDpi > 0 && targetDpi != qRound(curDpi)) {
                // 拖到屏幕边缘触发 Aero Snap 时，系统把拖动目标矩形改成贴边的
                // 半屏/全屏矩形，其中心可能已越过屏边界（本机副屏右边缘 x=0
                // 紧邻主屏原点）：此时用户意图是“在当前屏贴边”，不是跨屏。
                // 若据此切到目标屏 DPI，DPI 链会把窗口按比例缩放一次，松手后
                // 系统给出的 snap 尺寸又被覆盖（实测副屏右边缘 Snap 得到
                // 1720x1392 后立刻变回 800x533，半屏效果消失）。贴边矩形不参与
                // 跨屏判定，真跨屏仍由 WM_DPICHANGED 与 WM_EXITSIZEMOVE 兜底
                const bool edgeSnapped = mrc->left <= mi.rcMonitor.left
                        || mrc->right >= mi.rcMonitor.right || mrc->top <= mi.rcMonitor.top
                        || mrc->bottom >= mi.rcMonitor.bottom;
                if (edgeSnapped) {
                    isDpiDragPending = false;
                } else {
                    isDpiDragPending = true;
                    pendingDragDpi = targetDpi;
                    qDebug() << "WM_MOVING cross-dpi pending" << targetDpi;
                }
            } else if (targetDpi > 0) {
                // 拖回原 DPI 屏（或目标屏与当前同 DPI）：清除待处理状态
                isDpiDragPending = false;
            }
        }
        // 拖动标题栏移动/拖动还原过程中：lParam 为拖动目标矩形。
        // 目标宽度发生变化（最大化/半屏还原、Aero Snap、左右拉伸）时仅标记待重建，
        // 不在这里直接重建：系统拖拽模态循环中执行耗时重建会阻塞拖拽造成卡顿，
        // 且 Snap 时 Qt resizeEvent 尚未执行（chatShow 宽度未同步），重建会读旧宽度；
        // 高度变化不影响 MessageWidget 宽度，不触发重建
        if (dragRegenerateDone)
            break;
        RECT *rc = reinterpret_cast<RECT *>(msg->lParam);
        int w = rc->right - rc->left;
        if (w != dragStartWidth) {
            dragRegenerateDone = true;
            pendingRegenerateAfterResize = true;
            qDebug() << "WM_MOVING width changed, pending regenerate" << w;
        }
        break;
    }

    case WM_SIZING: {
        // 用户拖动窗口边缘拉伸（不是仅移动）：记录后仍交给默认处理
        userResizeDuringDrag = true;
        break;
    }

    case WM_EXITSIZEMOVE: {
        QTimer::singleShot(10, this, &MainWindow::applyDWMShadow);
        isSizeMoveDrag = false;
        isDpiDragPending = false;
        // 拖拽结束：顺带检查样式（缺失时延迟补齐）
        scheduleFrameStyleEnsure();

        // 标题栏拖动走 Windows 原生模态循环（WM_NCHITTEST 返回 HTCAPTION）：
        // 释放时 WM_LBUTTONUP 被模态循环消费，Qt 不派发 mouseReleaseEvent，
        // 其 screenChanged 分支不会执行；跨屏拖动结束的 DPI 缩放链必须在这里
        // 触发（mouseReleaseEvent 分支作为非模态拖动路径的补充保留）。
        // 判断条件除了 moveEvent 设置的 screenChanged 标志，还要比较当前屏幕
        // DPI 与 curDpi：Qt 平台层对 DPI 变化型跨屏移动会推迟屏幕切换（移动
        // 阶段 moveEvent 拿到的还是旧屏），屏幕在 WM_DPICHANGED 处理时才切换，
        // 而切换本身不触发新的 moveEvent，标志可能一直是 false——仅靠标志判断
        // 会漏掉跨 DPI 屏拖动，缩放链（字号重算 + 部件按比例缩放 + 消息重建）
        // 不执行，CodeEditor/WebEngineView 字号停留在旧屏像素值（Bug 2）。
        // applyScreenDpiChange 内部 newDpi==curDpi 时提前返回，同 DPI 屏间
        // 拖动不会产生多余重建
        const QScreen *releaseScreen = screen();
        const int releaseDpi = releaseScreen ? qRound(releaseScreen->logicalDotsPerInch()) : 0;
        if (dpiDragHandled) {
            // resizeEvent 拦截已执行过 DPI 缩放链（curDpi 已更新）：
            // 只清理 screenChanged 标志，不再重复缩放
            screenChanged = false;
        } else if (screenChanged || (releaseDpi > 0 && releaseDpi != qRound(curDpi))) {
            screenChanged = false;
            // 目标窗口尺寸由 dpiScaledSize 按物理尺寸基准换算：不依赖拖拽
            // 期间被系统临时缩放覆盖的 widgetSizeDict["MainWindow"]，也不
            // 依赖拖拽开始快照（来回跨屏时快照属于另一个 DPI，会算错）
            qDebug() << "WM_EXITSIZEMOVE screenChanged, applyScreenDpiChange" << releaseDpi;
            applyScreenDpiChange(releaseDpi);
        }
        // 注意：这里不能无条件按 DPI 基准校正窗口尺寸。Aero Snap 的最终矩形由
        // 系统在退出拖拽前后分两步应用（先给尺寸、位置稍后到位），最大化的还原
        // 也不由本消息决定，盲目 resize 会把半屏/贴边布局覆盖成普通尺寸（实测
        // 主屏左半屏 Snap 变回 1800x1200、副屏右半屏变回 800x533）。
        // 仅在“只移动”的跨 DPI 屏拖动后校正窗口尺寸（用户未同时拖边缘拉伸，
        // 且非最大化/非贴边）：此时最终尺寸是系统按“拖动过程中的临时尺寸 × 目标屏
        // DPI 比例”算出的。若拖动前窗口尺寸已被外部因素改过（缩放设置的拓扑重算
        // 临时改尺寸、程序化 MoveWindow）而与 DPI 基准不一致，这里会留下与基准
        // 不符的窗口（实测 1100x795@144 拖到副屏得到 733x530，而基准应为
        // 800x533），且该错误尺寸会被当作新基准导致后续尺寸漂移。贴边 Snap 的
        // 半屏尺寸由 isSnappedToScreenEdge 识别（含“系统先给尺寸、位置稍后到位”
        // 的中间帧），不会被这里覆盖
        if (dpiDragHandled && !userResizeDuringDrag && !isWindowMaximized()
                && !isSnappedToScreenEdge()) {
            const QSize expect = dpiScaledSize(curDpi);
            if (expect.isValid()
                    && (qAbs(width() - expect.width()) > 2
                        || qAbs(height() - expect.height()) > 2)) {
                qDebug() << "WM_EXITSIZEMOVE correct size by dpi base" << expect << "from"
                         << size();
                // 不置 avoidRepeatSelfFun：校正后的 resizeEvent 要重排 mainWidget
                // 与各部件，并在尺寸与基准一致时写回正确基准
                resize(expect);
            }
        }

        RECT rect;
        GetWindowRect(hwnd, &rect);
        int w = rect.right - rect.left;
        int h = rect.bottom - rect.top;
        if (pendingRegenerateAfterResize) {
            pendingRegenerateAfterResize = false;
            // 只比较宽度：仅高度变化的拉伸不需要重建 MessageWidget
            if (w != dragStartWidth) {
                // 拖拽结束、事件循环恢复后执行：此时 Qt resizeEvent 已同步新宽度，
                // 重建宽度正确且不阻塞拖拽过程（重建不在模态循环中执行）
                lastRegenerateWidth = w;
                lastRegenerateHeight = h;
                qDebug() << "WM_EXITSIZEMOVE regenerate pending" << w << h;
                scheduleMessageWidgetRegenerate();
            } else {
                lastRegenerateHeight = h;
                qDebug() << "WM_EXITSIZEMOVE pending canceled, width back to start" << w << h;
            }
        } else if (dragRegenerateDone) {
            // 拖动过程中已标记过；若最终宽度与重建时不同（继续拉伸）则补一次
            if (w != lastRegenerateWidth) {
                lastRegenerateWidth = w;
                lastRegenerateHeight = h;
                qDebug() << "WM_EXITSIZEMOVE width still changed, regenerate" << w << h;
                scheduleMessageWidgetRegenerate();
            } else {
                lastRegenerateHeight = h;
                qDebug() << "WM_EXITSIZEMOVE already regenerated, skip" << w << h;
            }
        } else if (w != dragStartWidth) {
            // 拖动中未触发（如 Snap 宽度变化发生在最后时刻）：现在重建
            lastRegenerateWidth = w;
            lastRegenerateHeight = h;
            qDebug() << "WM_EXITSIZEMOVE width changed, regenerate" << w << h;
            scheduleMessageWidgetRegenerate();
        } else {
            lastRegenerateHeight = h;
            qDebug() << "WM_EXITSIZEMOVE move or height only, skip regenerate" << w << h;
        }
        // 用户在拖拽中拉伸了窗口（当前尺寸与基准的 DPI 换算结果不符）：
        // 以当前尺寸为新基准，否则下次跨屏会跳回旧尺寸。
        // 贴边 Snap 的尺寸由系统布局给出，不代表用户设定的窗口尺寸：
        // 若把它写进基准，从半屏拖出后跨屏会按半屏尺寸等比放大
        if (!isWindowMaximized() && !isSnappedToScreenEdge()) {
            const QSize expect = dpiScaledSize(curDpi);
            if (qAbs(width() - expect.width()) > 2 || qAbs(height() - expect.height()) > 2) {
                baseWindowSize = size();
                baseWindowDpi = curDpi;
            }
        }
        dragRegenerateDone = false;
        break;
    }

    case WM_SYSCOMMAND: {
        break;
    }

    default:
        break;
    }
#endif
    return QMainWindow::nativeEvent(eventType, message, result);
}

// void MainWindow::mousePressEvent(QMouseEvent *event)
// {
//     qDebug() << "MainWindow::mousePressEvent";
//     QMainWindow::mousePressEvent(event);
//     if (event->button() == Qt::LeftButton) {
//         QPoint pos = event->pos();

//         int titleHeight = titleWidget->height();

//         if (pos.y() < titleHeight) {
//             int closeBtnWidth = titleWidget->getCloseButtonSize().width();
//             int maxBtnWidth = titleWidget->getMaxButtonSize().width();
//             int minBtnWidth = titleWidget->getMinButtonSize().width();
//             int btnRight = width();
//             int btnLeft = btnRight - closeBtnWidth - maxBtnWidth - minBtnWidth;

//             if (btnLeft < 0)
//                 btnLeft = 0;

//             if (pos.x() < btnLeft) {
// #ifdef Q_OS_WIN
//                 HWND hwnd = reinterpret_cast<HWND>(winId());
//                 if (hwnd) {
//                     ReleaseCapture();
//                     SendMessage(hwnd, WM_SYSCOMMAND, SC_MOVE | HTCAPTION, 0);
//                 }
// #endif
//                 return;
//             }
//         }
//     }
// }

// void MainWindow::mouseDoubleClickEvent(QMouseEvent *event)
// {
//     // QMainWindow::mouseDoubleClickEvent(event);
//     if (event->button() == Qt::LeftButton) {
//         QPoint pos = event->pos();
//         // QPoint pos = mapFromGlobal(event->globalPosition().toPoint());
//         // qDebug() << "MainWindow::mouseDoubleClickEvent" << pos << event->globalPosition().toPoint()
//         //          << mapFromGlobal(event->globalPosition().toPoint());
//         int titleHeight = titleWidget->height();

//         if (pos.y() < titleHeight) {
//             int closeBtnWidth = titleWidget->getCloseButtonSize().width();
//             int maxBtnWidth = titleWidget->getMaxButtonSize().width();
//             int minBtnWidth = titleWidget->getMinButtonSize().width();
//             int btnRight = width();
//             int btnLeft = btnRight - closeBtnWidth - maxBtnWidth - minBtnWidth;

//             if (btnLeft < 0)
//                 btnLeft = 0;

//             if (pos.x() < btnLeft) {
//                 qDebug() << "mouseDoubleClickEvent uiMaximize";
//                 uiMaximize();
//                 return;
//             }
//         }
//     }
// }

// void MainWindow::paintEvent(QPaintEvent *event)
// {
//     QMainWindow::paintEvent(event);

//     if (!isWindowMaximized()) {
//         QPainter painter(this);
//         painter.setRenderHint(QPainter::Antialiasing);

//         QPen pen(QColor(200, 200, 200, 180));
//         pen.setWidth(1);
//         painter.setPen(pen);
//         painter.setBrush(Qt::NoBrush);

//         QRectF rect = mainWidget->geometry().adjusted(0.5, 0.5, -0.5, -0.5);

//         if (rect.width() > 0 && rect.height() > 0) {
//             painter.drawRoundedRect(rect, 16, 16);
//         }
//     }
// }

void MainWindow::moveEvent(QMoveEvent *event)
{
    curScreen = screen();
    if (lastScreen != curScreen) {
        lastScreen = curScreen;
        screenChanged = true;
    } else if (curScreen && qRound(curScreen->logicalDotsPerInch()) != qRound(curDpi)) {
        // 跨 DPI 屏拖动时 Qt 在移动阶段仍返回旧屏（屏幕切换推迟到 WM_DPICHANGED
        // 处理，且切换不触发新的 moveEvent）；以 DPI 与 curDpi 的差异兜底置位，
        // 保证 mouseReleaseEvent/WM_EXITSIZEMOVE 的跨屏缩放分支能被触发
        screenChanged = true;
    }
    QMainWindow::moveEvent(event);
}

void MainWindow::onScreenChanged(QScreen *newScreen)
{
    if (!newScreen)
        return;
    const int dpi = qRound(newScreen->logicalDotsPerInch());
    qDebug() << "MainWindow::screenChanged" << newScreen->name() << "dpi" << dpi << "curDpi"
             << curDpi;
    if (dpi <= 0 || dpi == qRound(curDpi))
        return;
    // 拖拽中由 resizeEvent 的跨 DPI 拦截路径提前执行缩放链（WM_MOVING 检测
    // 到目标屏 DPI），释放时 WM_EXITSIZEMOVE 兜底，这里不重复处理
    if (isSizeMoveDrag)
        return;
    // 系统 DPI 设置变更（显示设置里改缩放比例）时，窗口在显示拓扑重算中会
    // 临时跨屏再回到原屏，期间按临时屏 DPI 执行过缩放链；回到原屏后没有
    // 可靠的 WM_DPICHANGED 时序，界面会停留在临时屏的字号/尺寸上（整体变大），
    // 此前需要点击一次才由 mouseReleaseEvent 的 screenChanged 分支纠正。
    // 延迟到本轮系统消息处理完（WM_DPICHANGED 建议矩形可能在本信号之后才
    // 应用到窗口），再按窗口实际所在屏 DPI 校正
    QTimer::singleShot(0, this, [this, dpi]() {
        if (isSizeMoveDrag)
            return;
        const QScreen *s = screen();
        const int nowDpi = s ? qRound(s->logicalDotsPerInch()) : 0;
        if (nowDpi > 0 && nowDpi == dpi && nowDpi != qRound(curDpi)) {
            screenChanged = false;
            applyScreenDpiChange(nowDpi);
        }
    });
}

void MainWindow::mouseMoveEvent(QMouseEvent *event)
{
    isItemShowFull(childAt(event->pos()));
    // QPoint cursorGlobalPos = event->globalPosition().toPoint();
    // cursorGlobalX = cursorGlobalPos.x();
    // cursorGlobalY = cursorGlobalPos.y();
    // uiGlobalTL = mainWidget->mapToGlobal(QPoint(0, 0));
    // uiGlobalBR = mainWidget->mapToGlobal(QPoint(mainWidget->width() - 1, mainWidget->height() - 1));
    // if (!mouseLeftButtonIsPress)
    //     regionDivision();
    // else {
    //     if ((regionDir != RegionEnum::Title) && (regionDir != RegionEnum::Button)
    //         && (regionDir != RegionEnum::Middle))
    //         UiStretch();
    //     else {
    //         if (regionDir == RegionEnum::Title) {
    //             UiDrag(event->globalPosition().toPoint());
    //             if (!(screens.size() > 1)) {
    //                 QRect screenGeometry = screen()->availableGeometry();
    //                 if (event->globalPosition().toPoint().x() <= screenGeometry.x()) {
    //                     if (!((width() == screenGeometry.width() / 2)
    //                           && (height() == screenGeometry.height()))) {
    //                         uiRectWidth = width();
    //                         uiRectHeight = height();
    //                         isChangeRectFirst = true;
    //                     }
    //                     qDebug() << "mouseMoveEvent screenGeometry:" << screenGeometry;
    //                     setGeometry(screenGeometry.x(), screenGeometry.y(),
    //                                 screenGeometry.width() / 2, screenGeometry.height());
    //                     mainWidget->setGeometry(0, 0, width(), height());
    //                     mainWidget->setStyleSheet("#mainWidget {"
    //                                               "    background-color: #F0F0F0;"
    //                                               "}");
    //                     titleWidget->setRightAngle();
    //                     isScreenHalf = true;
    //                 } else {
    //                     if (isChangeRectFirst) {
    //                         isChangeRectFirst = false;
    //                         resize(uiRectWidth, uiRectHeight);
    //                         mainWidget->setStyleSheet("#mainWidget {"
    //                                                   "    border-radius: 16px;"
    //                                                   "    background-color: #F0F0F0;"
    //                                                   "}");
    //                         titleWidget->setRoundAngle();
    //                     }
    //                 }
    //             }
    //         }
    //     }
    // }
    QMainWindow::mouseMoveEvent(event);
}

void MainWindow::isItemShowFull(QWidget *widget)
{
    for (int i = 0; i < messageWidgetList.size(); ++i) {
        MessageWidget *messageWidget = messageWidgetList.at(i);
        messageWidget->hideFunWidget();
    }
    if (TextWidget *textWidget = qobject_cast<TextWidget *>(widget)) {
        for (int i = 0; i < messageWidgetList.size(); ++i) {
            if (textWidget == messageWidgetList.at(i)->getTextWidget()) {
                messageWidgetList.at(i)->showFunWidget();
            }
        }
    } else if (TextBoxWidget *textBoxWidget = qobject_cast<TextBoxWidget *>(widget)) {
        for (int i = 0; i < messageWidgetList.size(); ++i) {
            if (textBoxWidget == messageWidgetList.at(i)->getTextBoxWidget()) {
                messageWidgetList.at(i)->showFunWidget();
            }
        }
    } else if (MessageWidget *messageWidget = qobject_cast<MessageWidget *>(widget)) {
        for (int i = 0; i < messageWidgetList.size(); ++i) {
            if (messageWidget == messageWidgetList.at(i)) {
                messageWidgetList.at(i)->showFunWidget();
            }
        }
    } else if (ItemWidget *itemWidget = qobject_cast<ItemWidget *>(widget)) {
        QLayoutItem *layoutItem = itemWidget->layout()->itemAt(0);
        if (layoutItem) {
            if (MessageWidget *childWidget = qobject_cast<MessageWidget *>(layoutItem->widget())) {
                for (int i = 0; i < messageWidgetList.size(); ++i) {
                    if (childWidget == messageWidgetList.at(i)) {
                        messageWidgetList.at(i)->showFunWidget();
                    }
                }
            }
        }
    }
}

// void MainWindow::regionDivision()
// {
//     if ((cursorGlobalX >= uiGlobalTL.x()) && (cursorGlobalX <= uiGlobalTL.x() + padding)
//         && (cursorGlobalY >= uiGlobalTL.y()) && (cursorGlobalY <= uiGlobalTL.y() + padding)) {
//         regionDir = RegionEnum::LeftTop;
//         setCursor(QCursor(Qt::SizeFDiagCursor));
//     } else if ((cursorGlobalX >= uiGlobalBR.x() - padding) && (cursorGlobalX <= uiGlobalBR.x())
//                && (cursorGlobalY >= uiGlobalTL.y())
//                && (cursorGlobalY <= uiGlobalTL.y() + padding)) {
//         regionDir = RegionEnum::RightTop;
//         setCursor(QCursor(Qt::SizeBDiagCursor));
//     } else if ((cursorGlobalX >= uiGlobalTL.x()) && (cursorGlobalX <= uiGlobalTL.x() + padding)
//                && (cursorGlobalY >= uiGlobalBR.y() - padding)
//                && (cursorGlobalY <= uiGlobalBR.y())) {
//         regionDir = RegionEnum::LeftBottom;
//         setCursor(QCursor(Qt::SizeBDiagCursor));
//     } else if ((cursorGlobalX >= uiGlobalBR.x() - padding) && (cursorGlobalX <= uiGlobalBR.x())
//                && (cursorGlobalY >= uiGlobalBR.y() - padding)
//                && (cursorGlobalY <= uiGlobalBR.y())) {
//         regionDir = RegionEnum::RightBottom;
//         setCursor(QCursor(Qt::SizeFDiagCursor));
//     } else if ((cursorGlobalX >= uiGlobalTL.x()) && (cursorGlobalX <= uiGlobalTL.x() + padding)) {
//         regionDir = RegionEnum::Left;
//         setCursor(QCursor(Qt::SizeHorCursor));
//     } else if ((cursorGlobalX >= uiGlobalBR.x() - padding) && (cursorGlobalX <= uiGlobalBR.x())) {
//         regionDir = RegionEnum::Right;
//         setCursor(QCursor(Qt::SizeHorCursor));
//     } else if ((cursorGlobalY >= uiGlobalTL.y()) && (cursorGlobalY <= uiGlobalTL.y() + padding)) {
//         regionDir = RegionEnum::Top;
//         setCursor(QCursor(Qt::SizeVerCursor));
//     } else if ((cursorGlobalY >= uiGlobalBR.y() - padding) && (cursorGlobalY <= uiGlobalBR.y())) {
//         regionDir = RegionEnum::Bottom;
//         setCursor(QCursor(Qt::SizeVerCursor));
//     } else if ((cursorGlobalX >= uiGlobalTL.x() + padding + 1)
//                && (cursorGlobalX <= uiGlobalBR.x() - padding - 1)
//                && (cursorGlobalY >= uiGlobalTL.y() + padding + 1)
//                && (cursorGlobalY <= uiGlobalTL.y() + titleWidget->height())) {
//         if (cursorGlobalX <= uiGlobalBR.x() - titleWidget->getMinButtonSize().width()
//                     - titleWidget->getMaxButtonSize().width()
//                     - titleWidget->getCloseButtonSize().width() - 1)
//             regionDir = RegionEnum::Title;
//         else
//             regionDir = RegionEnum::Button;
//         setCursor(QCursor(Qt::ArrowCursor));
//     } else {
//         regionDir = RegionEnum::Middle;
//         setCursor(QCursor(Qt::ArrowCursor));
//     }
// }

// void MainWindow::UiStretch()
// {
//     QRect uiGlobalRect(uiGlobalTL, uiGlobalBR);
//     switch (regionDir) {
//     case RegionEnum::Left:
//         if (uiGlobalBR.x() - cursorGlobalX
//             > minimumWidth() - 2 * widgetSizeDict["mainWidget x"].value<int>())
//             uiGlobalRect.setX(cursorGlobalX);
//         else
//             uiGlobalRect.setX(uiGlobalBR.x()
//                               - (minimumWidth() - 2 * widgetSizeDict["mainWidget x"].value<int>())
//                               + 1);
//         break;
//     case RegionEnum::Right:
//         if (cursorGlobalX - uiGlobalTL.x()
//             > minimumWidth() - 2 * widgetSizeDict["mainWidget x"].value<int>())
//             uiGlobalRect.setWidth(cursorGlobalX - uiGlobalTL.x());
//         else
//             uiGlobalRect.setWidth(minimumWidth() - 2 * widgetSizeDict["mainWidget x"].value<int>());
//         break;
//     case RegionEnum::Top:
//         if (uiGlobalBR.y() - cursorGlobalY
//             > minimumHeight() - 2 * widgetSizeDict["mainWidget y"].value<int>())
//             uiGlobalRect.setY(cursorGlobalY);
//         else
//             uiGlobalRect.setY(uiGlobalBR.y()
//                               - (minimumHeight() - 2 * widgetSizeDict["mainWidget y"].value<int>())
//                               + 1);
//         break;
//     case RegionEnum::Bottom:
//         if (cursorGlobalY - uiGlobalTL.y()
//             > minimumHeight() - 2 * widgetSizeDict["mainWidget y"].value<int>())
//             uiGlobalRect.setHeight(cursorGlobalY - uiGlobalTL.y());
//         else
//             uiGlobalRect.setHeight(minimumHeight()
//                                    - 2 * widgetSizeDict["mainWidget y"].value<int>());
//         break;
//     case RegionEnum::LeftTop:
//         if (uiGlobalBR.x() - cursorGlobalX
//             > minimumWidth() - 2 * widgetSizeDict["mainWidget x"].value<int>())
//             uiGlobalRect.setX(cursorGlobalX);
//         else
//             uiGlobalRect.setX(uiGlobalBR.x()
//                               - (minimumWidth() - 2 * widgetSizeDict["mainWidget x"].value<int>())
//                               + 1);
//         if (uiGlobalBR.y() - cursorGlobalY
//             > minimumHeight() - 2 * widgetSizeDict["mainWidget y"].value<int>())
//             uiGlobalRect.setY(cursorGlobalY);
//         else
//             uiGlobalRect.setY(uiGlobalBR.y()
//                               - (minimumHeight() - 2 * widgetSizeDict["mainWidget y"].value<int>())
//                               + 1);
//         break;
//     case RegionEnum::RightTop:
//         if (cursorGlobalX - uiGlobalTL.x()
//             > minimumWidth() - 2 * widgetSizeDict["mainWidget x"].value<int>())
//             uiGlobalRect.setWidth(cursorGlobalX - uiGlobalTL.x());
//         else
//             uiGlobalRect.setWidth(minimumWidth() - 2 * widgetSizeDict["mainWidget x"].value<int>());
//         if (uiGlobalBR.y() - cursorGlobalY
//             > minimumHeight() - 2 * widgetSizeDict["mainWidget y"].value<int>())
//             uiGlobalRect.setY(cursorGlobalY);
//         else
//             uiGlobalRect.setY(uiGlobalBR.y()
//                               - (minimumHeight() - 2 * widgetSizeDict["mainWidget y"].value<int>())
//                               + 1);
//         break;
//     case RegionEnum::LeftBottom:
//         if (uiGlobalBR.x() - cursorGlobalX
//             > minimumWidth() - 2 * widgetSizeDict["mainWidget x"].value<int>())
//             uiGlobalRect.setX(cursorGlobalX);
//         else
//             uiGlobalRect.setX(uiGlobalBR.x()
//                               - (minimumWidth() - 2 * widgetSizeDict["mainWidget x"].value<int>())
//                               + 1);
//         if (cursorGlobalY - uiGlobalTL.y()
//             > minimumHeight() - 2 * widgetSizeDict["mainWidget y"].value<int>())
//             uiGlobalRect.setHeight(cursorGlobalY - uiGlobalTL.y());
//         else
//             uiGlobalRect.setHeight(minimumHeight()
//                                    - 2 * widgetSizeDict["mainWidget y"].value<int>());
//         break;
//     case RegionEnum::RightBottom:
//         if (cursorGlobalX - uiGlobalTL.x()
//             > minimumWidth() - 2 * widgetSizeDict["mainWidget x"].value<int>())
//             uiGlobalRect.setWidth(cursorGlobalX - uiGlobalTL.x());
//         else
//             uiGlobalRect.setWidth(minimumWidth() - 2 * widgetSizeDict["mainWidget x"].value<int>());
//         if (cursorGlobalY - uiGlobalTL.y()
//             > minimumHeight() - 2 * widgetSizeDict["mainWidget y"].value<int>())
//             uiGlobalRect.setHeight(cursorGlobalY - uiGlobalTL.y());
//         else
//             uiGlobalRect.setHeight(minimumHeight()
//                                    - 2 * widgetSizeDict["mainWidget y"].value<int>());
//     default:
//         break;
//     }
//     QRect windowGlobalRect(uiGlobalRect.x() - widgetSizeDict["mainWidget x"].value<int>(),
//                            uiGlobalRect.y() - widgetSizeDict["mainWidget y"].value<int>(),
//                            uiGlobalRect.width() + 2 * widgetSizeDict["mainWidget x"].value<int>(),
//                            uiGlobalRect.height() + 2 * widgetSizeDict["mainWidget y"].value<int>());
//     setGeometry(windowGlobalRect);
// }

// void MainWindow::UiDrag(QPoint globalPos)
// {
//     move(pressPosDistanceUiGlobalTL + globalPos);
// }

// void MainWindow::mousePressEvent(QMouseEvent *event)
// {
//             if (event->button() == Qt::LeftButton) {
//             QPoint pos = event->pos();

//             if (pos.y() < titleWidget->height()) {
//                 int btnRight = width() - 10;
//                 int btnLeft = btnRight - titleWidget->getCloseButtonSize().width()
//                         - titleWidget->getMaxButtonSize().width()
//                         - titleWidget->getMinButtonSize().width() - 40;

//                 if (pos.x() < btnLeft) {
//     #ifdef Q_OS_WIN
//                     HWND hwnd = reinterpret_cast<HWND>(winId());
//                     if (hwnd) {
//                         ReleaseCapture();
//                         SendMessage(hwnd, WM_SYSCOMMAND, SC_MOVE | HTCAPTION, 0);
//                     }
//     #endif
//                     return;
//                 }
//             }
//         }
//         QMainWindow::mousePressEvent(event);
// }

// void MainWindow::mouseDoubleClickEvent(QMouseEvent *event)
// {
//     if (event->button() == Qt::LeftButton) {
//         QPoint pos = event->pos();
//         if (pos.y() < titleWidget->height()) {
//             int btnRight = width() - 10;
//             int btnLeft = btnRight - titleWidget->getCloseButtonSize().width()
//                     - titleWidget->getMaxButtonSize().width()
//                     - titleWidget->getMinButtonSize().width() - 40;
//             if (pos.x() < btnLeft) {
//                 uiMaximize();
//                 return;
//             }
//         }
//     }
//     QMainWindow::mouseDoubleClickEvent(event);
// }

void MainWindow::mouseReleaseEvent(QMouseEvent *event)
{
    // qDebug() << "mouseReleaseEvent isScreenMax" << isScreenMax;
    qDebug() << "mouseReleaseEvent MainWindow size:" << size();
    if (event->button() == Qt::LeftButton) {
        // mouseLeftButtonIsPress = false;
        // if (isScreenHalf) {
        //     QRect screenGeometry = screen()->availableGeometry();
        //     qDebug() << "mouseReleaseEvent screenGeometry:" << screenGeometry;
        //     QRect screenHalfRect = QRect(screenGeometry.x(), screenGeometry.y(),
        //                                  screenGeometry.width() / 2, screenGeometry.height());
        //     qDebug() << "mouseReleaseEvent screenHalfRect:" << screenHalfRect;
        //     if ((geometry().topLeft() != screenHalfRect.topLeft())
        //         || (geometry().width() != screenHalfRect.width())
        //         || (geometry().height() != screenHalfRect.height())) {
        //         if (!isScreenMax)
        //             isScreenHalf = false;
        //     }
        // }
        if (pushButtonIsPress)
            pushButtonIsPress = false;
        else {
            QRect chatInputRect;
            if (settingWidgetIsOpen)
                chatInputRect =
                        QRect(chatInput->geometry().x() + settingWidget->width(),
                              chatInput->geometry().y() + titleWidget->height() + chatFun->height()
                                      + chatShowWidget->height(),
                              chatInput->geometry().width(), chatInput->geometry().height());
            else if (chatRecordsWidgetIsOpen)
                chatInputRect =
                        QRect(chatInput->geometry().x() + chatRecordsWidget->width(),
                              chatInput->geometry().y() + titleWidget->height() + chatFun->height()
                                      + chatShowWidget->height(),
                              chatInput->geometry().width(), chatInput->geometry().height());
            else
                chatInputRect =
                        QRect(chatInput->geometry().x(),
                              chatInput->geometry().y() + titleWidget->height() + chatFun->height()
                                      + chatShowWidget->height(),
                              chatInput->geometry().width(), chatInput->geometry().height());
            if (chatInputRect.contains(event->pos()))
                chatInput->backgroundColorShowLight();
            else {
                chatInput->backgroundColorShowDark();
                chatInput->clearFocus();
            }
        }
        if (screenChanged) {
            qDebug() << "mouseReleaseEvent screenChanged";
            screenChanged = false;
            // 跨屏拖动结束：新屏 DPI 与旧屏不同时执行完整缩放链——重算像素字号、
            // 窗口与全部部件按 DPI 比例缩放（物理大小跨屏保持一致）、消息部件
            // （WebEngineView 页面与 CodeEditor）以新字号重建。
            // 用 Qt 维护的当前屏取值：缓存的 curScreen 可能因显示拓扑变化已被销毁
            const QScreen *releaseScreen = screen();
            if (releaseScreen)
                applyScreenDpiChange(qRound(releaseScreen->logicalDotsPerInch()));
        }
    }
    QMainWindow::mouseReleaseEvent(event);
}

void MainWindow::onDpiChanged()
{
    // 某块屏的 DPI（系统缩放设置）发生变化：按窗口实际所在屏的 DPI 处理
    // （curScreen 由 moveEvent 维护，在系统拓扑重算过渡期可能滞后）；窗口不在
    // 该屏时换算结果与 curDpi 相同，直接返回
    const QScreen *s = screen();
    const int dpi = s ? qRound(s->logicalDotsPerInch()) : 0;
    qDebug() << "onDpiChanged screenDpi" << dpi << "curDpi" << curDpi;
    if (dpi > 0 && dpi != qRound(curDpi))
        applyScreenDpiChange(dpi);
}

QSize MainWindow::dpiScaledSize(qreal dpi) const
{
    // 基准无效（窗口尚未完成初始布局）或 DPI 非法时以当前尺寸兜底
    if (!(baseWindowDpi > 0) || baseWindowSize.isEmpty() || !(dpi > 0))
        return size();
    return QSize(qRound(baseWindowSize.width() * dpi / baseWindowDpi),
                 qRound(baseWindowSize.height() * dpi / baseWindowDpi));
}

void MainWindow::applyLayoutDpiScale(qreal dpi)
{
    if (dpi <= 0)
        dpi = curDpi > 0 ? curDpi : qreal(DpiUtil::kBaseDpi);
    // 最小高度同样参与 DPI 换算：固定 244/100 在 96 DPI 屏上物理偏大，会把
    // splitter 的上 pane 卡得过高（窗口整体缩小时 chatShow 无法按比例变小）；
    // 216 DPI 屏上又挡不住过度压缩
    chatShowWidget->setMinimumHeight(DpiUtil::scalePx(kBaseChatShowMinHeight, dpi));
    chatInputWidget->setMinimumHeight(DpiUtil::scalePx(kBaseChatInputMinHeight, dpi));
    // 两个容器的内边距同样参与换算：chatShow 的左右/上下边距决定消息列表的
    // 可用宽度与高度，chatInput 的下边距计入 splitter 中 pane 的高度
    // （见 applyChatInputHeightForDpi），chatInput 的左右边距决定输入框宽度
    chatShowVLayout->setContentsMargins(DpiUtil::scaleMargins(kBaseChatShowMargins, dpi));
    chatInputVLayout->setContentsMargins(DpiUtil::scaleMargins(kBaseChatInputMargins, dpi));
    qDebug().nospace().noquote() << "applyLayoutDpiScale dpi=" << dpi
                                 << " chatShowMin=" << chatShowWidget->minimumHeight()
                                 << " chatInputMin=" << chatInputWidget->minimumHeight()
                                 << " chatShowMargins=" << chatShowVLayout->contentsMargins()
                                 << " chatInputMargins=" << chatInputVLayout->contentsMargins();
}

void MainWindow::applyChatInputHeightForDpi()
{
    // chatInput 的高度基准（144 DPI 屏的像素高度）尚未就绪或 splitter 尚未
    // 完成布局时跳过：下一次 resizeEvent / 首次显示的兜底会再试一次
    if (baseChatInputHeight <= 0 || splitter->height() <= 0)
        return;
    // chatInput 高度按 144 基准换算；其上下布局边距（chatInputVLayout）同样已
    // 按 144 基准换算（见 applyLayoutDpiScale），这里取当前值相加才能得到
    // chatInputWidget 的目标高度
    const QMargins inputMargins = chatInputVLayout->contentsMargins();
    const int paneHeight = DpiUtil::scalePx(baseChatInputHeight, curDpi)
            + inputMargins.top() + inputMargins.bottom();
    splitter->setSizes({ qMax(1, splitter->height() - paneHeight), paneHeight });
    qDebug().nospace().noquote() << "chatInput 高度=" << paneHeight << " dpi=" << curDpi
                                << " splitter=" << splitter->sizes();
    // pane 高度重设会改变 chatShowWidget 的高度，两个浮动提示标签的纵向位置以它为
    // 基准相减，必须在这里重算：resizeEvent 里那一次可能比 pane 高度先算（布局在
    // 窗口 resize 时还没激活、chatShowWidget 仍是旧高度），标签会停在错位的位置
    updateFloatingLabelPosition(curDpi);
}

void MainWindow::chatInputResizeGripPressed(int globalY)
{
    // 快照按下时的 pane 尺寸与鼠标位置：拖动中按位移一次性换算目标高度，
    // 不逐帧累加（逐帧取整会在跨越多个像素后产生可见误差）
    chatInputGripStartY = globalY;
    chatInputGripStartSizes = splitter->sizes();
}

void MainWindow::chatInputResizeGripMoved(int globalY)
{
    if (chatInputGripStartSizes.size() < 2)
        return;
    const int total = chatInputGripStartSizes.at(0) + chatInputGripStartSizes.at(1);
    if (total <= 0)
        return;
    // 向上拖（globalY 减小）为增高输入框
    const int startPane = chatInputGripStartSizes.at(1);
    const int minPane = qMax(1, chatInputWidget->minimumHeight());
    const int maxPane = qMax(minPane, total - qMax(1, chatShowWidget->minimumHeight()));
    const int pane = qBound(minPane, startPane - (globalY - chatInputGripStartY), maxPane);
    splitter->setSizes({ qMax(1, total - pane), pane });
}

void MainWindow::chatInputResizeGripReleased()
{
    chatInputGripStartSizes.clear();
    writeBackChatInputHeightBase();
}

void MainWindow::writeBackChatInputHeightBase()
{
    if (curDpi <= 0)
        return;
    const QList<int> sizes = splitter->sizes();
    if (sizes.size() < 2)
        return;
    // pane 高度包含 chatInputWidget 的上下内边距，扣掉才是 chatInput 自身高度
    const QMargins margins = chatInputVLayout->contentsMargins();
    const int inputHeight = sizes.at(1) - margins.top() - margins.bottom();
    if (inputHeight > 0)
        baseChatInputHeight = DpiUtil::toBasePx(inputHeight, curDpi);
}

void MainWindow::updateFloatingLabelPosition(qreal dpi)
{
    if (!emptyTextLabel || !textCopyLabel || !titleWidget || !chatFun || !chatShowWidget)
        return;
    if (dpi <= 0)
        dpi = curDpi > 0 ? curDpi : qreal(DpiUtil::kBaseDpi);
    // 位置以 144 DPI 屏的默认位置为基准：纵向 = 标题栏 + 功能栏 + 聊天显示区
    // 高度之和（三者由各自的 DPI 缩放链给出）+ 间距（代码中的固定像素，按基准
    // 换算）；横向居中随窗口宽度自动跟随，两个标签宽度由字体度量得出、同样跨
    // DPI 换算，所以居中后的物理位置在任一 DPI 屏上一致
    const int gap = DpiUtil::scalePx(kBaseFloatingLabelGap, dpi, 0);
    const int y = titleWidget->height() + chatFun->height()
            + chatShowWidget->height() + gap;
    emptyTextLabel->move((width() - emptyTextLabel->width()) / 2, y);
    textCopyLabel->move((width() - textCopyLabel->width()) / 2, y);
    qDebug().nospace().noquote() << "浮动提示标签位置 dpi=" << qRound(dpi) << " gap=" << gap
                                 << " y=" << y
                                 << " emptyTextLabel=" << emptyTextLabel->geometry()
                                 << " textCopyLabel=" << textCopyLabel->geometry();
}

void MainWindow::scheduleDpiSettleCheck()
{
    // 缩放设置变更期间系统会重建显示拓扑：窗口可能被临时移到另一块屏、Qt
    // 报告的屏/DPI 也会短暂错误。等本轮系统消息处理完再
    // 比较“实际所在屏 DPI”与已应用的 curDpi，不一致则自动校正一次；
    // 校正内部会重新调度本校验，直到两者一致（DPI 持续波动时也不会无限循环：
    // 只在确实不一致时才执行）
    QTimer::singleShot(800, this, [this]() {
        if (isSizeMoveDrag)
            return;
        const QScreen *s = screen();
        const int nowDpi = s ? qRound(s->logicalDotsPerInch()) : 0;
        if (nowDpi <= 0 || nowDpi == qRound(curDpi))
            return;
        qDebug() << "dpi settle check correct" << nowDpi << "curDpi" << curDpi;
        screenChanged = false;
        applyScreenDpiChange(nowDpi);
    });
}

void MainWindow::applyScreenDpiChange(int newDpi, bool scheduleRegenerate)
{
    qDebug() << "applyScreenDpiChange newDpi" << newDpi << "curDpi" << curDpi;
    if (newDpi == curDpi)
        return;
    lastDpi = curDpi;
    curDpi = newDpi;
    // 重建调度决策透传给 resizeEvent 的 isDpiChanged 链尾（见 dpiChainScheduleRegenerate）
    dpiChainScheduleRegenerate = scheduleRegenerate;
    // 像素字号按新屏 DPI 从磅值重算：全部像素字号控件重设字体后、WebEngineView
    // 页面与 CodeEditor 重建后统一生效；磅值字号控件（QToolTip 等）另行处理
    appContext->setWindowFontPixelSize(
            std::ceil(appContext->windowFontPointSize() * (curDpi / 72.0)));
    appContext->setTitleFontPixelSize(
            std::ceil(appContext->titleFontPointSize() * (curDpi / 72.0)));
    applyToolTipFont();
    isDpiChanged = true;
    // 两个分栏容器的最小高度与布局内边距先按新 DPI 更新（在 resize 之前）：
    // 它们会限制 splitter 的最小尺寸，滞后于 resize 会把窗口/子部件卡成错误尺寸
    applyLayoutDpiScale(curDpi);
    // 最小尺寸必须与窗口尺寸同时按新 DPI 更新，且必须在 resize 之前：系统在
    // 缩放设置变更后按建议矩形 resize 窗口时，若最小尺寸还是旧 DPI 的值，
    // 窗口会被旧最小值卡成错误尺寸（实测 216→96 时得到 1665x1195，而不是
    // 800x533），该帧内所有部件按错误宽度重排并把错误值写回基准。
    // 缓存里保存的是 144 基准值，这里按新 DPI 换算，不回写缓存（避免逐次
    // 换算累积取整误差）
    {
        const QSize minBase = widgetSizeDict["MainWindow minimumSize"].value<QSize>();
        if (minBase.isValid())
            setMinimumSize(DpiUtil::scaleSize(minBase, curDpi));
    }
    // 触发 resizeEvent 的 isDpiChanged 缩放链：按比例缩放窗口最小尺寸、主部件
    // 几何与各部件 updateSize，最后重建消息部件。窗口能按目标尺寸 resize 时由
    // 真实 resize 事件驱动；最大化或目标尺寸与当前相同（比例四舍五入后相等）时
    // 手动驱动一次缩放链，保证字体仍然更新
    const QSize target = dpiScaledSize(curDpi);
    // 最大化与贴边 Snap 时窗口几何由系统布局决定：按 DPI 比例 resize 会覆盖
    // 系统刚给出的尺寸（实测副屏右边缘 Snap 得到 1720x1392 后，DPI 链又把它
    // 缩成 800x533，只剩位置变化、半屏效果消失）。这两种情形只驱动布局与
    // 部件缩放，不改变窗口几何
    if (!isWindowMaximized() && !isSnappedToScreenEdge() && target != size()) {
        resize(target);
    } else {
        QResizeEvent e(size(), size());
        resizeEvent(&e);
    }
    // 兜底：缩放设置变更/拓扑重算期间 Qt 报告的屏与 DPI 可能短暂错误，
    // 稳定后再校验一次，不一致则自动校正（无需用户点击）
    scheduleDpiSettleCheck();
}

void MainWindow::resizeEvent(QResizeEvent *event)
{
    Q_UNUSED(event);
    // 本帧已在 DPI 链中按高度基准重新分配 chatInput 的 pane 高度：末尾不再回写
    // 基准（此时 chatInput 尚未按新 pane 高度重新布局，旧高度会污染基准）
    bool skipInputHeightBaseCapture = false;
    if (avoidRepeatSelfFun) {
        avoidRepeatSelfFun = false;
        return;
    }
    // 跨屏拖动中系统按 DPI 缩放窗口（WM_DPICHANGED）会触发本 resizeEvent：
    // 若不拦截，普通布局路径会把子部件按新窗口尺寸重新布局（如
    // SettingWidget::setSize 按新宽度重排，且 resetWidgetSize 会把新尺寸
    // 写回 widgetSizeDict），随后 WM_EXITSIZEMOVE 的 DPI 缩放链又按比例
    // 缩放一次——两次缩放叠加使部件物理尺寸在拖动前后变化过大（切换屏幕
    // 显示效果 Bug）。这里提前执行完整 DPI 缩放链（一次到位），释放时
    // WM_EXITSIZEMOVE 检测到 curDpi 已更新即跳过，不再重复缩放
    if (!isDpiChanged) {
        int targetDpi = 0;
        if (isDpiDragPending && pendingDragDpi != qRound(curDpi)) {
            // WM_MOVING 已用拖动目标矩形中心检测到目标屏 DPI（首选，
            // 时序上早于系统缩放窗口，最可靠）
            targetDpi = pendingDragDpi;
        } else {
            // 兜底：resizeEvent 时 screen() 已切换到新屏则按其 DPI 判断
            const QScreen *s = screen();
            const int screenDpi = s ? qRound(s->logicalDotsPerInch()) : 0;
            if (screenDpi > 0 && screenDpi != qRound(curDpi))
                targetDpi = screenDpi;
        }
        if (targetDpi > 0) {
            if (isSizeMoveDrag) {
                qDebug() << "resizeEvent dpi drag intercepted, applyScreenDpiChange" << targetDpi;
                // 目标窗口尺寸由 dpiScaledSize 按物理尺寸基准换算（不依赖拖拽前的
                // 临时尺寸快照：来回跨屏多次时快照属于另一个 DPI，乘以比例会被
                // 最小尺寸限制卡住，窗口与部件比例失常）
                // 重建不在拖动模态循环中执行（耗时重建会阻塞拖拽）：只标记
                // 待重建，由 WM_EXITSIZEMOVE 的 pending 分支在释放后调度
                applyScreenDpiChange(targetDpi, false);
                isDpiDragPending = false;
                pendingRegenerateAfterResize = true;
                dpiDragHandled = true;
                // 释放鼠标后才会重建消息部件（重建耗时，不能在拖拽模态循环中做），
                // 但未释放前也要让 MessageWidget 的字体与尺寸符合新屏比例：
                // 这里按新 DPI 做一次不重建的轻量刷新
                messageWidgetUpdateSize(curDpi, lastDpi);
            } else if (!isShowFirst) {
                // 非拖拽的跨屏：系统 WM_DPICHANGED 直接改窗口尺寸（显示缩放变更的
                // 拓扑重算、程序化移动窗口），或平台层已切换 screen() 但 curDpi 尚未
                // 更新。若放任普通布局路径先按新窗口尺寸重排一次，随后的 DPI 缩放链
                // 会再乘一遍 curDpi/lastDpi，子部件被缩放两次。
                // 这里与拖拽路径一样先执行完整 DPI 缩放链，不在模态循环中，
                // 可直接调度消息重建
                qDebug() << "resizeEvent dpi change intercepted, applyScreenDpiChange" << targetDpi;
                applyScreenDpiChange(targetDpi, true);
                isDpiDragPending = false;
            }
            if (isSizeMoveDrag || !isShowFirst)
                return;
        }
    }
    // 最小尺寸已在 applyScreenDpiChange 的 resize 之前按新 DPI 更新（见该处
    // 注释）：这里再乘一次 curDpi/lastDpi 会把最小值缩成二级小值
    // 本帧尺寸是否可以作为 DPI 基准（widgetSizeDict 与各部件 saveWidgetSize）：
    // 系统在“显示缩放设置变更”的拓扑重算中会先按新 DPI 改窗口尺寸（此时 Qt
    // 报告的 curDpi 还是旧值），该帧尺寸与 dpiScaledSize(curDpi) 对不上。若把
    // 它当基准，下一次 DPI 换算的比例就与基准所属 DPI 不一致，所有部件按错误
    // 比例缩小（实测 216→96 时基准被 192 DPI 的 1600x1064 按 96/216 算成
    // 711x473）。过渡帧只做布局、不写基准，下次真正按 DPI 缩放时比例才正确。
    // 用户拖拽拉伸、最大化、贴边 Snap 的尺寸另有语义，不参与该判定
    bool captureBase = true;
    if (!isSizeMoveDrag && !isWindowMaximized() && !isSnappedToScreenEdge()) {
        const QSize expect = dpiScaledSize(curDpi);
        if (expect.width() > 0
                && (qAbs(width() - expect.width()) > 2 || qAbs(height() - expect.height()) > 2))
            captureBase = false;
    }
    // qDebug() << "resizeEvent isScreenMax" << isScreenMax;
    if (isWindowMaximized())
        mainWidget->setGeometry(0, 0, width(), height());
    else {
        // 贴边 Snap 的窗口几何由系统布局给出，与 baseWindowSize 的 DPI 换算
        // 结果不成比例（实测副屏 Snap 右半屏为 1720x1392，而基准换算只有
        // 800x533）。此时若仍按 widgetSizeDict["mainWidget"] 乘 curDpi/lastDpi，
        // mainWidget 会被缩回基准尺寸，依赖其宽度的 chatFun/chatInput 一起变小
        // （newChatButton 出现在预期位置左边）；直到点最大化走上面按 width()
        // 铺满的分支才恢复。这里系统布局几何一律按当前窗口铺满，
        // 且不写回 DPI 基准（半屏尺寸不代表用户设定的尺寸）
        const bool systemGeometry = isSnappedToScreenEdge();
        // 按比例缩放只用于窗口尺寸与基准对不上的过渡帧（captureBase=false）：此时
        // 窗口尺寸不可信，只能按部件基准等比缩放。窗口尺寸已等于基准期望值时
        // （captureBase=true，包括 DPI 链驱动的那一帧）以窗口为准铺满，否则
        // mainWidget 会按“部件基准×比例”算出一个与窗口不符的尺寸（实测窗口
        // 1800x1200 时 mainWidget 只有 1200x800，chatFun 随之变窄）并停留到
        // 下一次交互才恢复
        const bool scaleByDpi = isDpiChanged && !systemGeometry && !captureBase;
        // mainWidget 的位置/尺寸缓存按 144 基准保存（见下方写回），这里统一
        // 按目标 DPI 从基准换算，不再乘 curDpi/lastDpi
        const int mx = DpiUtil::scalePx(widgetSizeDict["mainWidget x"].value<int>(), curDpi, 0);
        const int my = DpiUtil::scalePx(widgetSizeDict["mainWidget y"].value<int>(), curDpi, 0);
        if (scaleByDpi) {
            const QSize mainBaseSize = widgetSizeDict["mainWidget"].value<QSize>();
            mainWidget->setGeometry(
                    QRect(QPoint(mx, my), DpiUtil::scaleSize(mainBaseSize, curDpi)));
        } else {
            qDebug() << "mainWidget geometry" << mainWidget->geometry();
            mainWidget->setGeometry(mx, my, width() - 2 * mx, height() - 2 * my);
            qDebug() << "mainWidget x y" << mx << my;
            qDebug() << "mainWidget geometry" << mainWidget->geometry();
        }
        if (!systemGeometry && captureBase) {
            widgetSizeDict["mainWidget"] = DpiUtil::toBaseSize(mainWidget->size(), curDpi);
            widgetSizeDict["mainWidget x"] = DpiUtil::toBasePx(mainWidget->x(), curDpi);
            widgetSizeDict["mainWidget y"] = DpiUtil::toBasePx(mainWidget->y(), curDpi);
        }
    }
    if (isDpiChanged) {
        emptyTextLabel->updateSize(curDpi, lastDpi);
        textCopyLabel->updateSize(curDpi, lastDpi);
        settingWidget->updateSize(curDpi, lastDpi);
        titleWidget->updateSize(curDpi, lastDpi);
        chatRecordsWidget->updateSize(curDpi, lastDpi);
        chatFun->updateSize(curDpi, lastDpi);
        chatInput->updateSize(curDpi, lastDpi);
        // chatInput 的高度同样参与 DPI 缩放：splitter 在窗口 resize 时保持既有
        // 比例分配（stretch 0 的 chatInputWidget 不会按 chatInput 的 sizeHint
        // 自行换算），低 DPI 屏上输入框会保持 144 DPI 的像素高度、物理偏高。
        // 这里标记在末尾（布局已按新窗口尺寸重排后）按 144 基准重设 pane 高度
        if (baseChatInputHeight > 0)
            skipInputHeightBaseCapture = true;
    }
    settingWidget->resize(mainWidget->width() / 3, mainWidget->height() - titleWidget->height());
    chatRecordsWidget->resize(mainWidget->width() / 3,
                              mainWidget->height() - titleWidget->height());
    // 过渡帧（系统抢跑改了窗口尺寸、curDpi 还是旧值）只重排、不写基准：
    // resetWidgetSize 会把当前尺寸存进 widgetSizeDict，作为下一次 DPI 换算的基准
    if (captureBase)
        chatRecordsWidget->resetWidgetSize();
    else
        chatRecordsWidget->setSize();
    if (settingWidgetIsOpen || chatRecordsWidgetIsOpen) {
        // 展开态内容区宽度统一使用 mainWidget->width() - mainWidget->width() / 3：
        // 侧栏宽度为 mainWidget->width() / 3，chatRecordsUiAnimationMove 的终态与
        // chatShowWidth(true) 都是该表达式；若这里改用 * 2 / 3，当窗口宽度除 3 余 2 时
        // 会比其小 1px，展开后拖拉窗口将使 AI 最大宽度的右边缘与用户 ImageLabel
        // 的右边缘错开 1px
        const int contentWidth = mainWidget->width() - mainWidget->width() / 3;
        chatFun->setFixedSize(contentWidth, chatFun->height());
        if (captureBase)
            chatFun->resetWidgetSize();
        else
            chatFun->setSize();
        chatShow->resize(contentWidth - chatShowVLayout->contentsMargins().left()
                                - chatShowVLayout->contentsMargins().right(),
                        chatShow->height());
        chatShowWidget->resize(contentWidth, chatShowWidget->height());
        chatInput->resize(contentWidth - chatInputVLayout->contentsMargins().left()
                                  - chatInputVLayout->contentsMargins().right(),
                          chatInput->height());
        chatInput->resetWidgetSize();
        chatInputWidget->resize(contentWidth, chatInputWidget->height());
        splitter->resize(contentWidth, splitter->height());
        contentVLayout->setContentsMargins(mainWidget->width() / 3, 0, 0, 0);
        if (settingWidgetIsOpen)
            settingWidget->move(0, titleWidget->height());
        else
            settingWidget->move(-settingWidget->width(), titleWidget->height());
        if (chatRecordsWidgetIsOpen)
            chatRecordsWidget->move(0, titleWidget->height());
        else
            chatRecordsWidget->move(-chatRecordsWidget->width(), titleWidget->height());
    } else {
        chatFun->setFixedSize(mainWidget->width(), chatFun->height());
        if (captureBase)
            chatFun->resetWidgetSize();
        else
            chatFun->setSize();
        // qDebug() << "mainWindow size:" << size();
        chatInput->resetWidgetSize();
        settingWidget->move(-settingWidget->width(), titleWidget->height());
        chatRecordsWidget->move(-chatRecordsWidget->width(), titleWidget->height());
    }
    if (isDpiChanged)
        settingWidget->updateLayoutMarginsSpacing(curDpi, lastDpi);
    if (captureBase) {
        settingWidget->resetWidgetSize();
        titleWidget->saveWidgetSize();
    } else {
        settingWidget->setSize();
    }
    updateFloatingLabelPosition(curDpi);
    // 同 DPI 的横向拉伸（拖拽中未释放鼠标）不经过任何消息部件宽度刷新路径：
    // MessageWidget 的最大宽度只取决于窗口宽度，不刷新则气泡宽度停留在拖拽前的
    // 窗口宽度，直到释放鼠标后的完整重建才恢复。
    // 这里按节流做一次不重建的轻量刷新（只重设最大宽度与子控件宽度/字号，
    // 不重载 WebEngine 页面），使气泡宽度在拖拽中即随窗口同步变化
    if (isSizeMoveDrag && !isDpiChanged)
        scheduleMessageWidthRefresh();
    if (isDpiChanged) {
        isDpiChanged = false;
        const QSize dpiTarget = dpiScaledSize(curDpi);
        // 贴边/最大化时窗口尺寸由系统布局决定，链尾不再按 DPI 基准覆盖
        if (dpiTarget != size() && !isWindowMaximized() && !isSnappedToScreenEdge()) {
            avoidRepeatSelfFun = true;
            resize(dpiTarget.width(), dpiTarget.height());
        }
        // 与上方 resize 触发的 WM_SIZE 重建请求经统一调度入口合并去重；
        // 拖动中提前执行本缩放链时不立即调度（重建耗时，阻塞拖拽模态循环），
        // 重建由调用方（WM_EXITSIZEMOVE）在释放后统一触发
        if (dpiChainScheduleRegenerate)
            scheduleMessageWidgetRegenerate();
    }
    // 注意：这里不再用当前尺寸回写 DPI 物理尺寸基准——系统在缩放设置变更的
    // 拓扑重算、窗口临时跨屏期间会主动改窗口尺寸（且尺寸与其报告的 DPI 可能
    // 不匹配），把它当作基准会让后续换算一路放大；基准只在用户拖拽开始/结束
    // （WM_ENTERSIZEMOVE / WM_EXITSIZEMOVE）与构造时更新。缓存同样按 144 基准保存
    widgetSizeDict["MainWindow"] = DpiUtil::toBaseSize(size(), curDpi);
    qDebug() << "resizeEvent MainWindow size:" << size();
    // 跨屏/缩放回归诊断：一行汇总本帧的 DPI、窗口与关键子部件尺寸（探针脚本
    // test/_textinput_dpi_check/*.ps1 从日志中提取该行比对 144/96 两档期望值）
    qDebug().nospace().noquote() << "诊断 dpi " << qRound(curDpi) << " last " << qRound(lastDpi)
                                 << " size " << size() << " capture " << captureBase
                                 << " snapped " << isSnappedToScreenEdge() << " mainWidget "
                                 << mainWidget->size() << " chatFun " << chatFun->size();
    // chatInput 高度参与 DPI 缩放：本帧已按 144 基准重设 pane 高度；布局可能还
    // 待在事件循环里激活（此时 splitter 高度仍是旧值、sizes 会被按比例重排），
    // 再兜底一次（按基准绝对换算，重复执行结果不变）
    if (skipInputHeightBaseCapture) {
        applyChatInputHeightForDpi();
        QTimer::singleShot(0, this, &MainWindow::applyChatInputHeightForDpi);
    }
    // qDebug() << "resizeEvent lastNormalGeometry:" << lastNormalGeometry;
}

bool MainWindow::isSnappedToScreenEdge() const
{
    // Aero Snap（贴左/右/上/下边缘的半屏、四分之一屏）与贴边最大化：窗口矩形
    // 紧贴当前屏工作区的某条边，尺寸由系统布局决定，DPI 缩放链不得再按比例
    // resize 覆盖（否则半屏尺寸被换成基准尺寸）。容差 2px 吸收边框取整误差。
    // 判定必须是“与边界的距离在容差内”，不能写成 g.left() <= work.left() + 容差：
    // 窗口位于另一块屏（本机副屏在 x<0 一侧）而 Qt 报告的 screen() 在显示缩放
    // 设置的拓扑重算中短暂滞后为副屏右侧的主屏时，该条件恒为真，被误判成贴边
    // Snap——resizeEvent 因此跳过 DPI 基准写回，基准停留在上上个 DPI 的尺寸，
    // 下一次换算按错误比例缩小（实测 216→96 时 mainWidget 1600x1064 被按
    // 96/216 算成 711x473，chatFun 变为 711x36、设置面板 237x445、输入框高 201）。
    // 同理也不允许“窗口大半在别的屏上”时按该屏边界判贴边
    const QScreen *s = screen();
    const QRect g = geometry();
    if (!g.isValid())
        return false;
    // 归属屏：优先取 screen()，但它在本项目里并不可靠——跨屏拖动、缩放设置
    // 变更的拓扑重算期间 Qt 报告的屏可能滞后（窗口已经落到新屏，screen() 仍是
    // 旧屏），此时窗口与“该屏工作区”完全不相交。实测副屏拖到主屏左边缘 Snap
    // 成功得到 1280x1528 后，DPI 链按旧屏判贴边失败，把半屏改回基准 1200x800。
    // 因此两者不相交时退化为“与窗口重叠面积最大的屏”
    QRect work;
    QRect inter;
    if (s) {
        work = s->availableGeometry();
        inter = work.isValid() ? g.intersected(work) : QRect();
    }
    if (!s || inter.isEmpty()) {
        int bestArea = 0;
        const QList<QScreen *> allScreens = QGuiApplication::screens();
        for (const QScreen *c : allScreens) {
            if (!c)
                continue;
            const QRect cw = c->availableGeometry();
            const QRect ci = g.intersected(cw);
            if (ci.isEmpty())
                continue;
            const int area = ci.width() * ci.height();
            if (area > bestArea) {
                bestArea = area;
                work = cw;
                inter = ci;
            }
        }
        if (bestArea <= 0)
            return false;
    }
    // 窗口横跨两屏时以重叠面积判归属：在两个方向上都不到该屏一半，忽略
    if (inter.width() * 2 < g.width() && inter.height() * 2 < g.height())
        return false;
    constexpr int kTolerance = 2;
    const bool edgeHit = qAbs(g.left() - work.left()) <= kTolerance
            || qAbs(g.top() - work.top()) <= kTolerance
            || qAbs(g.right() - work.right()) <= kTolerance
            || qAbs(g.bottom() - work.bottom()) <= kTolerance;
    if (edgeHit)
        return true;
    // 跨 DPI 屏拖到边缘 Snap 时，系统分两步：先给出 Snap 尺寸，位置稍后才到位。
    // 该中间帧位置不贴边（实测副屏拖到主屏左边缘时得到 QRect(-666,680 1280x1528)，
    // 而主屏工作区为 0,0 2560x1528），仅靠边界距离判不出来，DPI 链就会把半屏
    // 尺寸改回基准尺寸、Snap 失效。这里补上尺寸判据：窗口尺寸等于某块屏工作区
    // 的一半（左/右、上/下、四分之一）或整块工作区时，也属于系统布局结果。
    // 代价是用户手动把窗口调到恰好半屏大小时同样不参与 DPI 缩放，影响极小
    const QList<QScreen *> allScreens = QGuiApplication::screens();
    for (const QScreen *c : allScreens) {
        if (!c)
            continue;
        const QSize w = c->availableGeometry().size();
        if (g.size() == w || g.size() == QSize(w.width() / 2, w.height())
                || g.size() == QSize(w.width(), w.height() / 2)
                || g.size() == QSize(w.width() / 2, w.height() / 2))
            return true;
    }
    return false;
}

bool MainWindow::isWindowMaximized() const
{
    return windowState() & Qt::WindowMaximized;
}

void MainWindow::uiMaximize()
{
    qDebug() << "MainWindow::uiMaximize";
    if (isWindowMaximized()) {
        showNormal();
        // titleWidget->maxButtonToggleIcon(true);
    } else {
        showMaximized();
        // titleWidget->maxButtonToggleIcon(false);
    }
}

void MainWindow::uiMinimize()
{
    showMinimized();
}

void MainWindow::uiClose()
{
    saveCurChatRecord(true);
    close();
}

void MainWindow::settingButtonClicked()
{
    settingWidget->raise();
    settingAnimationMove->setStartValue(settingWidget->geometry());
    settingAnimationMove->setEndValue(
            QRect(0, titleWidget->height(), settingWidget->width(), settingWidget->height()));
    settingAnimationMove->start();
    settingWidgetIsOpen = true;
    pushButtonIsPress = true;
}

void MainWindow::chatRecordsUiAnimationMove(const QVariant &value)
{
    QRect rect = value.toRect();
    chatFun->setFixedSize(mainWidget->width() - rect.x() - chatRecordsWidget->width(),
                          chatFun->height());
    chatFun->setSize();
    chatShow->resize(mainWidget->width() - rect.x() - chatRecordsWidget->width()
                             - chatShowVLayout->contentsMargins().left()
                             - chatShowVLayout->contentsMargins().right(),
                     chatShow->height());
    chatShowWidget->resize(mainWidget->width() - rect.x() - chatRecordsWidget->width(),
                           chatShowWidget->height());
    chatInput->resize(mainWidget->width() - rect.x() - chatRecordsWidget->width()
                              - chatInputVLayout->contentsMargins().left()
                              - chatInputVLayout->contentsMargins().right(),
                      chatInput->height());
    chatInput->resetWidgetSize();
    chatInputWidget->resize(mainWidget->width() - rect.x() - chatRecordsWidget->width(),
                            chatInputWidget->height());
    splitter->resize(mainWidget->width() - rect.x() - chatRecordsWidget->width(),
                     splitter->height());
    contentVLayout->setContentsMargins(rect.x() + chatRecordsWidget->width(), 0, 0, 0);
    const int count = qMin(chatShow->count(), messageWidgetList.size());
    // qDebug() << "chatRecordsUiAnimationMove rect:" << rect;
    for (int i = 0; i < count; ++i) {
        QListWidgetItem *item = chatShow->item(i);
        if (!item)
            continue;
        ItemWidget *itemWidget = qobject_cast<ItemWidget *>(chatShow->itemWidget(item));
        MessageWidget *messageWidget = messageWidgetList.at(i);
        if (messageWidget->getIsUser()) {
            if (QLayout *itemLayout = itemWidget->layout()) {
                if (chatRecordsWidgetIsOpen) {
                    // const int w = chatShow->width();
                    // const int h = messageWidget->height() + 10;
                    // itemWidget->setFixedSize(w, h);
                    itemLayout->setContentsMargins(
                            itemWidget->width() - rect.x() - chatRecordsWidget->width()
                                    - messageWidget->width() - itemRightMargin,
                            itemVerticalMargin,
                            itemRightMargin + rect.x() + chatRecordsWidget->width(),
                            itemVerticalMargin);
                } else {
                    const int w = chatShow->width();
                    const int h = messageWidget->height() + 10;
                    itemWidget->setFixedSize(w, h);
                    itemLayout->setContentsMargins(
                            itemWidget->width() - messageWidget->width() - itemRightMargin,
                            itemVerticalMargin, itemRightMargin, itemVerticalMargin);
                }
            }
        }
    }
}

void MainWindow::chatRecordsUiMoveFinished()
{
    chatFun->saveWidgetSize();
    if (!chatRecordsWidgetIsOpen)
        chatRecordsWidget->delAllListItems();
    // 展开/收起只改变 chatShow 宽度：MessageWidget 的最大宽度与展开状态无关
    // （由 messageWidgetMaxWidth() 统一计算），因此不重建控件，只按新宽度调整
    // itemRecvHLayout 等的边距；延迟到布局激活后执行，保证读到最终的 chatShow 宽度
    // QTimer::singleShot(0, this, &MainWindow::messageWidgetItemRelayout);
}

int MainWindow::chatShowWidth(bool recordsExpanded) const
{
    // 按展开状态计算而不是读取 chatShow->width()：展开/收起动画过程中、
    // 以及动画刚结束时布局尚未激活的情况下，读到的宽度并不一致，
    // 会导致同一窗口宽度下创建的 MessageWidget 宽度不同
    int left = 0, top = 0, right = 0, bottom = 0;
    if (chatShowVLayout)
        chatShowVLayout->getContentsMargins(&left, &top, &right, &bottom);
    // 聊天记录部件（或设置部件）展开时内容区只占 mainWidget 宽度的 2/3，
    // 与 resizeEvent 中 contentVLayout 的左边距保持一致
    const int contentWidth =
            recordsExpanded ? mainWidget->width() - mainWidget->width() / 3 : mainWidget->width();
    return contentWidth - left - right;
}

int MainWindow::messageWidgetMaxWidth() const
{
    // 最大宽度与聊天记录部件的展开状态无关（否则展开前后创建的 MessageWidget
    // 宽度不一致），取下列两者的较小值：
    // 1）未展开时 chatShow 宽度的 2/3；
    // 2）展开后 chatShow 宽度 - itemRightMargin：保证展开后 AI 消息达到最大宽度时，
    //    其右边缘（左边距 0 + 最大宽度）正好落在 chatShow 宽度 - itemRightMargin 处，
    //    与用户消息 ImageLabel 的右边缘在垂直方向上对齐，
    //    因此展开/收起只需调整 itemRecvHLayout 的边距，MessageWidget 不用重建
    const int maxWidth = qMin(chatShowWidth(false) * 2 / 3, chatShowWidth(true) - itemRightMargin);
    qDebug() << "minMessageWidgetMaxWidth:" << minMessageWidgetMaxWidth << "maxWidth:" << maxWidth;
    return qMax(minMessageWidgetMaxWidth, maxWidth);
}

void MainWindow::updateItemLayout(QWidget *itemWidget, MessageWidget *messageWidget,
                                  QListWidgetItem *item)
{
    if (!itemWidget || !messageWidget)
        return;
    const int w = chatShow->width();
    const int h = messageWidget->height() + 10;
    qDebug() << "updateItemLayout" << itemWidget << "msg:" << messageWidget << "w:" << w
             << "h:" << h;
    itemWidget->setFixedSize(w, h);
    if (QLayout *itemLayout = itemWidget->layout()) {
        if (messageWidget->getIsUser())
            // 用户消息右对齐：ImageLabel 右边缘固定在 chatShow 宽度 - itemRightMargin 处
            itemLayout->setContentsMargins(itemWidget->width() - messageWidget->width()
                                                   - itemRightMargin,
                                           itemVerticalMargin, itemRightMargin, itemVerticalMargin);
        else
            // AI 消息左边距恒为 0：达到最大宽度时右边缘与用户消息 ImageLabel 右边缘对齐，
            // 未达到最大宽度时同样使用该边距公式（不强制右对齐）
            itemLayout->setContentsMargins(0, itemVerticalMargin,
                                           itemWidget->width() - messageWidget->width(),
                                           itemVerticalMargin);
    }
    if (item)
        item->setSizeHint(QSize(w, h));
}

void MainWindow::messageWidgetItemRelayout()
{
    // 聊天记录部件展开/收起只改变 chatShow 宽度，而 MessageWidget 的最大宽度
    // 与展开状态无关，因此无需重建控件（重建耗时且会与流式追加的文本竞争），
    // 只需按新宽度同步各 item 部件的尺寸与布局边距
    const int count = qMin(chatShow->count(), messageWidgetList.size());
    for (int i = 0; i < count; ++i) {
        QListWidgetItem *item = chatShow->item(i);
        if (!item)
            continue;
        updateItemLayout(chatShow->itemWidget(item), messageWidgetList.at(i), item);
    }
    qDebug() << "messageWidgetItemRelayout chatShow width:" << chatShow->width()
             << "messageWidget maxWidth:" << messageWidgetMaxWidth() << "count:" << count;
}

void MainWindow::messageWidgetUpdateSize(qreal curDpi, qreal lastDpi)
{
    // 跨 DPI 屏拖动未释放鼠标前：消息部件的完整重建要等释放后执行（重建耗时，
    // 在拖拽模态循环中执行会阻塞拖拽），但不刷新则消息内文本/代码块仍按旧屏
    // 字号与宽度显示，与窗口及其他部件比例不符；这里只重设字体与尺寸
    if (lastDpi <= 0 || curDpi <= 0 || messageWidgetList.isEmpty())
        return;
    const int maxWidth = messageWidgetMaxWidth();
    for (auto *messageWidget : messageWidgetList) {
        if (messageWidget)
            messageWidget->updateSize(curDpi, lastDpi, maxWidth);
    }
    // 各 item 部件与布局边距按新尺寸同步（与展开/收起走同一条不重建的路径）
    messageWidgetItemRelayout();
    qDebug() << "MainWindow messageWidgetUpdateSize" << curDpi << lastDpi << "count"
             << messageWidgetList.size() << "maxWidth" << maxWidth;
}

void MainWindow::scheduleMessageWidthRefresh()
{
    // 拖拽中 WM_SIZE/resizeEvent 触发非常频繁，逐次对全部消息部件重新量测
    // 会使拖拽卡顿：合并到定时器超时后执行一次；延迟也保证执行时布局已激活
    // （chatShow 宽度已同步到新窗口宽度）
    if (messageWidthRefreshTimer.isActive())
        return;
    messageWidthRefreshTimer.start();
}

void MainWindow::refreshMessageWidthDuringDrag()
{
    // 拖拽已结束（释放后由 messageWidgetRegenerate 完整重建）或正处在 DPI
    // 缩放链中时不做轻量刷新
    if (!isSizeMoveDrag || isDpiChanged || messageWidgetList.isEmpty())
        return;
    const int maxWidth = messageWidgetMaxWidth();
    // 仅上下拉伸/宽度取整后未变化：气泡宽度无需更新，跳过文本重新量测
    if (maxWidth == lastMessageWidthRefreshMaxWidth)
        return;
    lastMessageWidthRefreshMaxWidth = maxWidth;
    // 同一 DPI（ratio=1）：只按新窗口宽度重设最大宽度与子控件尺寸/字号，
    // 不重建控件、不重载 WebEngine 页面
    messageWidgetUpdateSize(curDpi, curDpi);
    qDebug() << "MainWindow refreshMessageWidthDuringDrag maxWidth" << maxWidth;
}

void MainWindow::messageWidgetResize(MessageWidget *selfMessageWidget)
{
    qDebug() << "messageWidgetResize start" << selfMessageWidget;
    const int count = qMin(chatShow->count(), messageWidgetList.size());
    int i = 0;
    for (; i < count; ++i) {
        if (selfMessageWidget == messageWidgetList.at(i))
            break;
    }
    for (; i < count; ++i) {
        QListWidgetItem *item = chatShow->item(i);
        QWidget *itemWidget = chatShow->itemWidget(item);
        if (!itemWidget)
            return;
        updateItemLayout(itemWidget, messageWidgetList.at(i), item);
    }
    qDebug() << "messageWidgetResize end";
    // 本轮发送的消息量测完成（item 已改用真实高度）：解除自动滚动挂起，
    // 随后 updateItemLayout 触发的真实 rangeChanged 再执行正确的自动跟随
    if (selfMessageWidget == messageSendWidget)
        chatShow->setPendingSendItem(false);
}

void MainWindow::sendMessage()
{
    if (!isSending) {
        QList<QVariantMap> context = {};
        QString text = chatInput->toPlainText().trimmed();
        if (!text.isEmpty()) {
            // 用户主动发送消息：恢复滚动条自动跟随，避免之前滚动查看历史关闭的
            // 自动跟随（scrollAutoChange=false）残留使本轮新消息不再滚动到底部
            chatShow->resetScrollAutoChange();
            // 新消息 item 入列时尺寸未量测（临时高度）：挂起自动滚动，
            // 防止滚到"假底部"后视口只剩空白 item 区（消息消失再出现）
            chatShow->setPendingSendItem(true);
            for (auto *w : messageWidgetList) {
                QVariantMap m;
                m["role"] = w->getIsUser() ? "user" : "assistant";
                m["content"] = w->getText();
                context.append(m);
            }
            thinkTimeLengthList.append(0);
            messageSendWidget = new MessageWidget(
                    appContext, text, [this]() { textCopy(); },
                    [this]() { messageRenewResponse(); },
                    [this](MessageWidget *selfMessageWidget) {
                        messageWidgetResize(selfMessageWidget);
                    },
                    [this](bool state) { getSetTexting(state); }, [this]() { onExecuteNext(); },
                    chatShow, thinkTimeLengthList, messageWidgetList.size(), true, true,
                    messageWidgetMaxWidth());
            messageSendWidget->hide();
            messageSendWidget->updateFunWidgetSize(curDpi, initDpi);
            // messageSendWidget->connectResizeFinished(this, &MainWindow::messageWidgetResize);
            // messageSendWidget->connectSetTexting(this, &MainWindow::getSetTexting);
            // messageSendWidget->connectExecuteNext(this, &MainWindow::onExecuteNext);
            // messageSendWidget->toggleWidget();
            messageWidgetList.append(messageSendWidget);

            itemSendWidget = new ItemWidget(this);
            itemSendHLayout = new QHBoxLayout(itemSendWidget);
            itemSendHLayout->addWidget(messageSendWidget);
            itemSendWidget->setFixedSize(chatShow->width(), messageSendWidget->height() + 10);

            sendItem = new QListWidgetItem(chatShow);
            sendItem->setSizeHint(QSize(chatShow->width(), messageSendWidget->height() + 10));
            chatShow->setItemWidget(sendItem, itemSendWidget);
            updateItemLayout(itemSendWidget, messageSendWidget, sendItem);

            thread = new MessageThread(appContext, text, context);
            chatInput->clearText();
            chatInput->setSending(true);
            isSending = true;
            isThreadFinished = false;
            // 用户主动发送新消息：新一轮对话重新开始，复位空文本自动重试计数
            emptyRenewCount = 0;
            qDebug() << "sendMessage:" << text;
        } else {
            emptyTextLabel->printStart();
        }
    } else {
        if (thread) {
            thread->stop();
            // 用户主动停止：结束空文本自动重新响应链（即使未达次数上限）
            emptyRenewCount = 0;
        }
        isSending = false;
        // 主动停止线程：放弃本轮收尾，防止积压队列清空时误触发 messageFinish
        isThreadFinished = false;
        if (messageRecvWidget)
            messageRecvWidget->breakHandle();
        qDebug() << "sendMessage isSending";
    }
}

void MainWindow::onExecuteNext()
{
    qDebug() << "onExecuteNext";
    // 会话切换后旧发送控件已销毁（resetRecvChain 已置空）：跳过，避免悬空访问
    if (!messageSendWidget)
        return;
    messageSendWidget->show();
    QTimer::singleShot(50, this, &MainWindow::startThread);
}

void MainWindow::startThread()
{
    connect(thread, &QThread::started, this, &MainWindow::messageStart);
    connect(thread, &MessageThread::newMessage, this, &MainWindow::queueMessage);
    // 线程完成信号不能作为触发收尾的唯一条件：此时 messageQueue 可能仍有积压
    // （渲染滞后或重建期间队列冻结），需等队列清空（所有文本已渲染）后才触发
    connect(thread, &QThread::finished, this, &MainWindow::onThreadFinished);
    thread->start();
    qDebug() << "startThread";
}

void MainWindow::messageStart()
{
    // 列表末尾已是本轮接收占位（文本为 null 的 AI 控件）时不重复创建：
    // 窗口重建的补建场景（messageWidgetRegenerate 内 messageStart 调用）后，
    // 本轮线程的 started 信号可能迟到派发到本槽。已入库过的 AI 消息文本
    // 至少为空串（非 null），故 null 文本 AI 控件 ⟺ 未入库的本轮占位
    if (!messageWidgetList.isEmpty()) {
        MessageWidget *lastMessageWidget = messageWidgetList.last();
        if (lastMessageWidget && !lastMessageWidget->getIsUser()
            && lastMessageWidget->getText().isNull())
            return;
    }
    message.clear();
    // 新接收开始：会话切换时 resetRecvChain 置 false 的渲染开关在此恢复，
    // 否则切换后新发送的流式文本不会渲染
    isContinueShow = true;
    int i = messageWidgetList.size() - 1;
    // i > 0：列表为空（i = -1）或仅一项时无前置消息可处理；重建恢复接收占位
    // （messageWidgetRegenerate）时可能在空列表上调用本函数
    if (i > 0) {
        if (messageWidgetList[i]->getIsUser())
            messageWidgetList[i - 1]->removeRenewResponseButton();
        else
            messageWidgetList[i]->removeRenewResponseButton();
    }

    thinkTimeLengthList.append(0);
    messageRecvWidget = new MessageWidget(
            appContext, message, [this]() { textCopy(); }, [this]() { messageRenewResponse(); },
            [this](MessageWidget *selfMessageWidget) { messageWidgetResize(selfMessageWidget); },
            [this](bool state) { getSetTexting(state); }, nullptr, chatShow, thinkTimeLengthList,
            messageWidgetList.size(), false, true, messageWidgetMaxWidth());
    // messageRecvWidget->connectResizeFinished(this, &MainWindow::messageWidgetResize);
    // messageRecvWidget->connectSetTexting(this, &MainWindow::getSetTexting);
    messageWidgetList.append(messageRecvWidget);

    itemRecvWidget = new ItemWidget(this);
    itemRecvHLayout = new QHBoxLayout(itemRecvWidget);
    itemRecvHLayout->addWidget(messageRecvWidget);
    itemRecvWidget->setFixedSize(chatShow->width(), messageRecvWidget->height() + 10);

    recvItem = new QListWidgetItem(chatShow);
    recvItem->setSizeHint(QSize(chatShow->width(), messageRecvWidget->height() + 10));
    chatShow->setItemWidget(recvItem, itemRecvWidget);
    updateItemLayout(itemRecvWidget, messageRecvWidget, recvItem);

    first = true;
    qDebug() << "messageStart";

    ensureFrameStyle("messageStart");
}

void MainWindow::queueMessage(const QString &text)
{
    messageQueue.enqueue(text);
    // 重建期间只入队不启动处理：待重建完成（全量刷新 setText 结束）后
    // 由 messageWidgetRegenerate 统一恢复队列，避免流式增量 setText 与全量刷新竞争
    if (!isProcessing && !isRegenerating) {
        isProcessing = true;
        // 启动时处理队列头（最旧的积压文本）而非新入队的 text：重建完成瞬间
        // （isRegenerating=false 后、恢复逻辑置 isProcessing=true 前）若线程信号
        // 先触发本函数，直接渲染新文本会跳过积压文本，导致新文本先累积到 message、
        // 积压文本后处理，渲染顺序错乱；处理队列头保证 FIFO 顺序，积压文本先渲染、
        // 新文本最后渲染
        recvMessage(messageQueue.head());
    }
}

void MainWindow::recvMessage(const QString &text)
{
    // 重建期间暂停队列处理：文本保留在队列中（不累积、不 dequeue），
    // 重建完成后由 messageWidgetRegenerate 统一恢复，避免流式增量 setText
    // 与全量刷新 setText 交错竞争
    if (isRegenerating) {
        isProcessing = false;
        return;
    }
    // 会话切换/新建聊天已清空队列：挂起的排队回调（singleShot）在切换后仍可能执行，
    // 此时无待渲染文本，直接退出，避免后续对空队列 dequeue 的未定义行为
    if (messageQueue.isEmpty()) {
        isProcessing = false;
        return;
    }
    qDebug() << "recvMessage:" << text;
    // QSignalBlocker blocker(thread);
    // isProcessing = true;
    if (first) {
        first = false;
        if (text.startsWith("\n "))
            message = text.mid(2);
        else
            message = text;
    } else {
        message += text;
    }

    if (isContinueShow && messageRecvWidget) {
        messageRecvWidget->setText(message);
        updateItemLayout(itemRecvWidget, messageRecvWidget, recvItem);
    }

    ensureFrameStyle("recvMessage");

    qDebug() << "recvMessage: setText finish";
    // setText 内部嵌套事件循环可能已触发会话切换（用户点击聊天记录）：
    // 队列已被 resetRecvChain 清空时跳过 dequeue
    if (!messageQueue.isEmpty())
        messageQueue.dequeue();
    qDebug() << "recvMessage: messageQueue dequeue";
    if (!messageQueue.isEmpty()) {
        QString next = messageQueue.head();
        QTimer::singleShot(0, this, [this, next]() { recvMessage(next); });
    } else {
        isProcessing = false;
        qDebug() << "recvMessage: isProcessing false";
        // 队列已清空：若线程已完成信号也已到达，此时才满足收尾触发条件
        // （两者缺一不可），补全文本并移除 loading
        if (isThreadFinished) {
            messageFinish();
        }
    }
}

void MainWindow::onThreadFinished()
{
    // 仅处理当前线程：会话切换后旧线程的 finished 信号可能迟到（thread 已指向新对象），
    // 忽略旧线程的完成信号，避免误触发收尾破坏新接收状态
    if (sender() != thread)
        return;
    // 线程完成信号到来后不能立即收尾：messageQueue 可能仍有未渲染的文本
    // （AI 输出速度大于渲染速度，或重建期间队列冻结）。仅标记线程已完成，
    // 待 recvMessage 处理完最后一条（队列为空）时再触发 messageFinish
    isThreadFinished = true;
    if (messageQueue.isEmpty()) {
        messageFinish();
    }
}

void MainWindow::messageFinish()
{
    // 切换聊天记录/新建聊天/重建进行中不执行收尾：MessageThread::stop()
    // （terminate+wait）会同步派发 finished 信号，使本函数在
    // applyChatRecordSwitch 中途被提前调用，此时旧控件尚未中止渲染，
    // 收尾操作（补全 setText/布局更新）会与随后的清空销毁流程交错
    if (isSwitchingChatRecord || isRegenerating) {
        if (isRegenerating)
            isSending = false;
        return;
    }
    // 接收真正结束（本函数由线程 finished 信号触发，是补全缺失文本的合适时机）：
    // 重建期间到达的追加文本只累积在 message 未渲染，重建完成后控件会缺少尾部文本，
    // 此处统一补全；此时接收已停止，不会与增量 setText 交错
    if (messageRecvWidget && isContinueShow && messageRecvWidget->getText() != message) {
        messageRecvWidget->setText(message);
        updateItemLayout(itemRecvWidget, messageRecvWidget, recvItem);
        ensureFrameStyle("messageFinish");
    }

    if (messageRecvWidget) {
        messageRecvWidget->removeLoadingWidget();
        messageRecvWidget->updateFunWidgetSize(curDpi, initDpi);
        // messageRecvWidget->toggleWidget();
    }

    if (messageRecvWidget && itemRecvWidget && itemRecvHLayout && recvItem) {
        updateItemLayout(itemRecvWidget, messageRecvWidget, recvItem);
    }

    // 本轮接收结束：复位发送状态。必须先于下方空回复重试判断执行——重试分支
    // （messageRenewResponse）会以新一轮开始的状态重新置位，若在其之后复位会
    // 把重试轮的发送状态清掉
    chatInput->setSending(false);
    isSending = false;

    // 空回复置占位移除：仅在接收控件与本轮 item 上有效。会话切换后这些指针已置空
    // （resetRecvChain），此时 message 必然被清空，若不加限制会误删新会话的最后一条消息
    if (message.isEmpty() && messageRecvWidget && itemRecvWidget && recvItem
        && !messageWidgetList.isEmpty() && chatShow->count() > 0) {
        // 不能同步 delete：本函数可能被渲染期间到达的线程完成信号触发，控件仍停留在
        // setText/buildAiUi 的嵌套事件循环（loop.exec）中，同步销毁会使其渲染栈访问
        // 已释放成员。改用 abortRendering：中止渲染等待、摘除父级并接管销毁时机
        // （渲染栈已退出则立即 deleteLater，否则推迟到渲染栈退出后）
        MessageWidget *lastMessageWidget = messageWidgetList.takeLast();
        if (lastMessageWidget)
            lastMessageWidget->abortRendering();
        int last = chatShow->count() - 1;
        QWidget *itemWidget = chatShow->itemWidget(chatShow->item(last));
        if (itemWidget)
            itemWidget->deleteLater();
        QListWidgetItem *lastItem = chatShow->takeItem(last);
        delete lastItem;
        // 空文本自动重试：次数上限与"用户主动停止即结束"的判定统一在
        // messageRenewResponse(isAutoRetry=true) 内处理（含用户停止拦截）
        messageRenewResponse(true);
    }

    qDebug() << "chatShow item count:" << chatShow->count();
    // const int count = chatShow->count();
    // for (int i = 0; i < count; ++i) {
    //     MessageWidget *messageWidget = messageWidgetList.at(i);
    //     qDebug() << i << "messageWidget size:" << messageWidget->size();
    // }
}

void MainWindow::resetRecvChain()
{
    // 切换聊天记录/新建聊天时调用：旧消息控件随后会被 deleteLater/chatShow->clear()
    // 销毁，但排队的 recvMessage 回调（singleShot）与尚未返回的 setText 调用栈
    // （流式渲染中嵌套事件循环内触发的切换）仍持有旧控件指针。这里清空积压队列、
    // 复位流式状态并断开接收链上的裸指针：
    // - recvMessage 通过 isContinueShow/messageRecvWidget 判断跳过对旧控件的渲染；
    // - 队列清空后，recvMessage 的空队列检查使挂起回调安全退出
    //   （避免对空队列 dequeue 的未定义行为）；
    // - messageFinish 通过 messageRecvWidget 判断避免误收尾
    // 先中止各控件的渲染等待栈：用户点击发生在旧控件的嵌套事件循环（loop.exec）内时，
    // 其 setText/buildAiUi 仍在同步等待子控件渲染（子控件随后的销毁会使等待条件
    // 永远无法满足，且渲染栈继续访问已销毁成员会悬空崩溃），使其在销毁前安全退出
    for (MessageWidget *messageWidget : messageWidgetList) {
        if (messageWidget)
            messageWidget->abortRendering();
    }
    messageQueue.clear();
    isProcessing = false;
    isContinueShow = false;
    isThreadFinished = false;
    // 会话切换/新建聊天：结束上一会话的空文本自动重试链
    emptyRenewCount = 0;
    first = true;
    message.clear();
    messageRecvWidget = nullptr;
    itemRecvWidget = nullptr;
    itemRecvHLayout = nullptr;
    recvItem = nullptr;
    messageSendWidget = nullptr;
    itemSendWidget = nullptr;
    itemSendHLayout = nullptr;
    sendItem = nullptr;
}

void MainWindow::textCopy()
{
    // 显示"文本复制成功"提示（2 秒后自动隐藏）
    textCopyLabel->printStart();
    // 复制按钮点击的 mouse release 事件会冒泡到窗口（CopyButton 中 e->ignore()），
    // 置标志使 mouseReleaseEvent 跳过聊天输入框的背景/焦点重置处理
    pushButtonIsPress = true;
}

void MainWindow::messageRenewResponse(bool isAutoRetry)
{
    // 空文本自动重试（AI 返回空回复时的自动重新响应）的链控制与计数：
    // - 自动来源：用户主动停止（stop() 已调用）立即结束重试链，即使未达次数
    //   上限；达到上限后不再重试（计数保持，待下一次用户交互入口复位）
    // - 用户主动来源（点击重新生成按钮/外部显式调用）：复位自动重试计数
    if (isAutoRetry) {
        // 用 isStoppedByUser 而非 QThread::isInterruptionRequested：后者在线程结束
        // （finished）后恒返回 false（Qt 内部还要求 running && !finished），而本检查
        // 恰好发生在 finished 驱动的收尾阶段（messageFinish），用中断标志会使停止
        // 拦截永远失效（停止后仍自动重试到上限）
        if (thread && thread->isStoppedByUser()) {
            // 用户主动停止：结束本轮空文本自动重试链
            qDebug() << "messageRenewResponse: stopped by user, end empty auto renew";
            emptyRenewCount = 0;
            return;
        }
        if (emptyRenewCount >= maxEmptyRenewCount) {
            // 达到次数上限：结束重试（保留计数，防御异常路径再次触发）
            qDebug() << "messageRenewResponse: empty auto renew limit reached"
                     << maxEmptyRenewCount;
            return;
        }
        emptyRenewCount++;
        qDebug() << "messageRenewResponse: empty auto renew" << emptyRenewCount << "/"
                 << maxEmptyRenewCount;
    } else {
        emptyRenewCount = 0;
    }

    // 重新生成响应：从消息列表末尾向前找最后一条用户消息，以该消息文本重新
    // 发起请求（其之前的历史作为上下文）；末尾已完成的 AI 回复仅跳过、不删除，
    // 新接收开始时由 messageStart 移除其重新生成按钮
    int i = messageWidgetList.size() - 1;
    int j = 0;
    QList<QVariantMap> context = {};
    while (i >= j) {
        if (messageWidgetList[i - j]->getIsUser()) {
            for (int k = 0; k < i - j; k++) {
                MessageWidget *messageWidget = messageWidgetList[k];
                QVariantMap m;
                m["role"] = messageWidget->getIsUser() ? "user" : "assistant";
                m["content"] = messageWidget->getText();
                context.append(m);
            }

            // create thread
            thread = new MessageThread(appContext, messageWidgetList[i - j]->getText(), context);
            // 新一轮接收开始：状态与 sendMessage 一致（发送按钮转为停止、复位
            // 线程完成标志——否则首条文本渲染后队列清空会误触发上一轮收尾）
            chatInput->setSending(true);
            isSending = true;
            isThreadFinished = false;
            // 用户主动重新生成：恢复滚动条自动跟随（与 sendMessage 一致），
            // 避免此前滚动查看历史关闭的自动跟随使新输出不滚动到底部
            // chatShow->resetScrollAutoChange();
            startThread();
            break;
        } else {
            j += 1;
        }
    }
    // 按钮点击的 mouse release 事件会冒泡到窗口：置标志跳过输入框背景处理（同 textCopy）
    pushButtonIsPress = true;
}

void MainWindow::createChatRecordSession(bool withholdCurSession,
                                         const QList<ChatMessageRecord> &messages)
{
    if (!chatRecordStore || !chatRecordStore->isOpen())
        return;

    const int sessionId = chatRecordStore->createSession();
    qDebug() << "createChatRecordSession sessionId:" << sessionId;
    if (sessionId < 0)
        return;

    chatRecordStore->saveMessages(sessionId, messages);

    if (!withholdCurSession) {
        curChatSessionId = sessionId;
    }
}

void MainWindow::saveCurChatRecord(bool withholdCurSession)
{
    // 消息列表正在按会话构建（generateCurChatRecord 内的嵌套事件循环期间）或上一次
    // 构建被连续点击中止时，messageWidgetList 只有部分消息，此时序列化会把当前会话
    // 记录重写成半成品：构建期间到达的保存请求（窗口关闭、打开记录列表、窗口重建等）
    // 一律跳过，数据库保持上一次完整保存的内容
    if (isBuildingChatRecord || isChatRecordBuildAbandoned)
        return;

    if (!chatRecordStore || !chatRecordStore->isOpen())
        return;

    if (messageWidgetList.size() != 0) {
        // 生成当前应写入的完整消息列表（思考时长与消息一一对应）
        QList<ChatMessageRecord> messages;
        messages.reserve(messageWidgetList.size());
        for (int i = 0; i < messageWidgetList.size(); i++) {
            MessageWidget *messageWidget = messageWidgetList.at(i);
            if (!messageWidget)
                continue;
            ChatMessageRecord record;
            record.content = messageWidget->getText();
            record.isUser = messageWidget->getIsUser();
            // null 字符串的 AI 消息是空文本自动重试期间的占位控件（文本从未接收、
            // 从未被赋值），跳过收集不入库；显式为空字符串 "" 的消息照常收集
            // （null 与 "" 语义不同：null 绑定 SQL NULL 会违反 content NOT NULL
            // 约束，兜底归一化见 ChatRecordStore::saveMessages）
            if (record.content.isNull() && !record.isUser)
                continue;
            record.thinkTime = i < thinkTimeLengthList.size() ? thinkTimeLengthList.at(i) : 0;
            messages.append(record);
        }
        if (messages.isEmpty())
            return;

        // 按完整内容比较后仅从差异处重写的策略在 ChatRecordStore::saveMessages 内：
        // 流式追加常为行内增长（消息数量不变），仅按数量判断会漏写，重建后将缺少
        // 最后追加的文本
        if (curChatSessionId >= 0 && chatRecordStore->sessionExists(curChatSessionId)) {
            chatRecordStore->saveMessages(curChatSessionId, messages);
        } else {
            createChatRecordSession(withholdCurSession, messages);
        }
    }
}

void MainWindow::chatRecordsGenerateItem(QString searchText)
{
    QList<ChatSessionRecord> sessions;
    if (chatRecordStore && chatRecordStore->isOpen())
        sessions = chatRecordStore->listSessions(searchText);

    // 会话按创建时间升序返回（即时间序），addListItem 头部插入后最新记录在最上方
    for (const ChatSessionRecord &session : sessions) {
        QListWidgetItem *item = chatRecordsWidget->addListItem(session.title);
        chatRecordsWidget->listItemSetData(item, QString::number(session.id));
    }
}

void MainWindow::generateCurChatRecord(bool lastIsToggle, bool useThinkExpandList)
{
    // 构建期间 messageWidgetList 为半成品：控件构造会进入渲染等待的嵌套事件循环，
    // 其中的点击/关闭等事件若触发保存会把会话记录截断，此标记供 saveCurChatRecord 拦截
    isBuildingChatRecord = true;

    QString text;
    bool isUser = true;
    thinkTimeLengthList.clear();
    int thinkTimeIndex = 0;
    int expandIndex = 0;
    // 本次构建是否被待执行的切换点击或待执行的重建请求中止（列表为半成品）
    bool buildAborted = false;

    // 按会话加载完整消息：数据库按 seq 顺序返回，无需再解析文本标记行
    QList<ChatMessageRecord> messages;
    if (chatRecordStore && chatRecordStore->isOpen() && curChatSessionId >= 0)
        messages = chatRecordStore->loadMessages(curChatSessionId);

    for (int i = 0; i < messages.size(); i++) {
        const ChatMessageRecord &record = messages.at(i);
        text = record.content;
        isUser = record.isUser;
        // 思考时长与消息一一对应，先入列表保证索引与消息序号一致（MessageWidget 构造时读取）
        thinkTimeLengthList.append(record.thinkTime);

        int j = messageWidgetList.size() - 1;
        if (j != -1 && j != 0) {
            if (!messageWidgetList[j]->getIsUser()) {
                messageWidgetList[j]->removeRenewResponseButton();
            }
        }

        qDebug() << "generateCurChatRecord" << text;
        if (useThinkExpandList) {
            if (!isUser) {
                bool thinkExpand = true;
                if (expandIndex < thinkExpandedList.size())
                    thinkExpand = thinkExpandedList[expandIndex];
                messageWidget = new MessageWidget(
                        appContext, text, [this]() { textCopy(); },
                        [this]() { messageRenewResponse(); },
                        [this](MessageWidget *selfMessageWidget) {
                            messageWidgetResize(selfMessageWidget);
                        },
                        [this](bool state) { getSetTexting(state); }, nullptr, chatShow,
                        thinkTimeLengthList, thinkTimeIndex, isUser, thinkExpand,
                        messageWidgetMaxWidth());
            } else {
                // 重建恢复历史消息，不传 executeNextFun，避免 TextShow 渲染完成后自动触发新线程
                messageWidget = new MessageWidget(
                        appContext, text, [this]() { textCopy(); },
                        [this]() { messageRenewResponse(); },
                        [this](MessageWidget *selfMessageWidget) {
                            messageWidgetResize(selfMessageWidget);
                        },
                        [this](bool state) { getSetTexting(state); }, nullptr, chatShow,
                        thinkTimeLengthList, thinkTimeIndex, isUser, true, messageWidgetMaxWidth());
            }
        } else {
            // 重建恢复历史消息，不传 executeNextFun，避免 TextShow 渲染完成后自动触发新线程
            messageWidget = new MessageWidget(
                    appContext, text, [this]() { textCopy(); },
                    [this]() { messageRenewResponse(); },
                    [this](MessageWidget *selfMessageWidget) {
                        messageWidgetResize(selfMessageWidget);
                    },
                    [this](bool state) { getSetTexting(state); }, nullptr, chatShow,
                    thinkTimeLengthList, thinkTimeIndex, isUser, true, messageWidgetMaxWidth());
        }
        // messageWidget->connectResizeFinished(this, &MainWindow::messageWidgetResize);
        // messageWidget->connectSetTexting(this, &MainWindow::getSetTexting);

        qDebug() << "generateCurChatRecord" << i;
        if (!isUser) {
            if (i == messages.size() - 1) {
                if (lastIsToggle) {
                    messageWidget->removeLoadingWidget();
                }
            } else {
                messageWidget->removeLoadingWidget();
            }
        }

        messageWidgetList.append(messageWidget);

        itemWidget = new ItemWidget(this);
        itemHLayout = new QHBoxLayout();
        itemHLayout->addWidget(messageWidget);
        itemWidget->setLayout(itemHLayout);
        itemWidget->setFixedSize(chatShow->width(), messageWidget->height() + 10);
        item = new QListWidgetItem(chatShow);
        item->setSizeHint(QSize(chatShow->width(), messageWidget->height() + 10));
        chatShow->setItemWidget(item, itemWidget);
        updateItemLayout(itemWidget, messageWidget, item);

        if (i == messages.size() - 1) {
            if (lastIsToggle) {
                messageWidget->updateFunWidgetSize(curDpi, initDpi);
                // messageWidget->toggleWidget();
            }
        } else {
            messageWidget->updateFunWidgetSize(curDpi, initDpi);
            // messageWidget->toggleWidget();
        }

        thinkTimeIndex++;
        if (useThinkExpandList && !isUser) {
            expandIndex++;
        }

        // 构建期间又收到切换点击（快速连续点击）：当前目标的列表已无展示价值，
        // 立即中止本次构建，把剩余时间交给待执行目标（半成品列表由下方标记
        // 阻止写回数据库，由待执行切换统一清空重建）
        if (hasPendingChatSwitch) {
            buildAborted = true;
            qDebug() << "generateCurChatRecord aborted, pending:" << pendingChatSessionId;
            break;
        }

        // 构建期间又收到窗口尺寸变化触发的重建请求（拖拽释放/最大化等）：当前
        // 构建的控件宽度已过期，立即中止本次构建，把剩余时间交给最新一次重建：
        // 待重建标记由 messageWidgetRegenerate 的 do-while 在重建轮结束时消费
        // （再执行一轮完整重建），此处即刻退出避免旧宽度列表无谓生成到底
        if (isRegeneratePending) {
            buildAborted = true;
            qDebug() << "generateCurChatRecord aborted, regenerate pending";
            break;
        }
    }

    isBuildingChatRecord = false;
    // 中止的构建（列表为半成品，不代表任何会话的完整内容）：阻止 saveCurChatRecord
    // 将其写回数据库；正常构建完成时清除标记
    isChatRecordBuildAbandoned = buildAborted;
}

void MainWindow::getSetTexting(bool state)
{
    Q_UNUSED(state);
}

void MainWindow::scheduleMessageWidgetRegenerate()
{
    // 已有排队等待执行的重建回调：本次请求合并进去不再重复调度。
    // 队列中堆积多个回调时，后执行的回调会在重建执行期间进入重入保护再次标记
    // 待重建，使重建完成后被追加执行一轮（同一次窗口尺寸变化被重复完整重建）；
    // 合并后由唯一回调在事件循环空闲时执行，重建时读取最新窗口宽度
    if (isRegenerateScheduled) {
        qDebug() << "scheduleMessageWidgetRegenerate merged";
        return;
    }
    isRegenerateScheduled = true;
    qDebug() << "scheduleMessageWidgetRegenerate";
    QTimer::singleShot(0, this, [this]() {
        isRegenerateScheduled = false;
        messageWidgetRegenerate();
    });
}

void MainWindow::messageWidgetRegenerate()
{
    // 重入保护：正在重建、AI 消息渲染中（嵌套事件循环内 WM_EXITSIZEMOVE 会被再次分发），
    // 或正在切换聊天记录/新建聊天（消息列表为半成品，重建会与之交错）。
    // 再次触发时仅标记待重建：当前重建轮由 generateCurChatRecord 的中止检查即刻
    // 终止（不再完整生成到底），do-while 随后以最新尺寸完整重建一次
    if (isRegenerating || isSwitchingChatRecord) {
        isRegeneratePending = true;
        return;
    }
    isRegenerating = true;
    // 重建已按当前窗口宽度生成全部消息部件：上次拖拽轻量刷新记录的最大宽度
    // 不再有效，否则首次拖拽到相同宽度时会误判“宽度未变化”而跳过刷新
    lastMessageWidthRefreshMaxWidth = -1;
    do {
        isRegeneratePending = false;
        if (messageWidgetList.size() != 0) {
            thinkExpandedList.clear();

            if (isSending) {
                isContinueShow = false;
            }

            if (!isSending) {
                currentScrollValue = chatShow->verticalScrollBar()->value();
                maxScrollValue = chatShow->verticalScrollBar()->maximum();
            }

            saveCurChatRecord();
            for (int i = 0; i < messageWidgetList.size(); i++) {
                MessageWidget *messageWidget = messageWidgetList[i];
                if (!messageWidget->getIsUser()) {
                    thinkExpandedList.append(messageWidget->getThinkIsExpanded());
                }
            }

            // 重建前记录本轮接收控件（列表最后一项）是否已渲染过文本：非 null 时
            // 占位消息已保存入库（重建后列表末尾的 AI 控件即其重建版），下方
            // isSending 分支可恢复接收指针；null 时占位消息被保存过滤
            // （saveCurChatRecord 跳过 null 的 AI 消息），重建后本轮控件不存在
            bool recvWidgetHasText = false;
            if (!messageWidgetList.isEmpty()) {
                MessageWidget *recvWidget = messageWidgetList.last();
                recvWidgetHasText = recvWidget && !recvWidget->getText().isNull();
            }

            // 重建期间置空接收指针，防止嵌套事件循环中消息回调访问旧控件
            // 先中止各控件的渲染等待栈（同 resetRecvChain）：窗口重建由嵌套事件循环
            // 内的 WM_EXITSIZEMOVE 触发时，旧控件的 setText/buildAiUi 仍在等待子控件
            // 渲染，控件销毁后渲染栈继续访问悬空成员会崩溃
            for (MessageWidget *oldMessageWidget : messageWidgetList) {
                if (oldMessageWidget)
                    oldMessageWidget->abortRendering();
            }
            messageRecvWidget = nullptr;
            messageWidgetList.clear();

            for (int i = 0; i < chatShow->count(); i++) {
                QWidget *itemWidget = chatShow->itemWidget(chatShow->item(i));
                if (itemWidget)
                    itemWidget->deleteLater();
            }
            chatShow->clear();

            if (!isSending) {
                generateCurChatRecord(true, true);
                QTimer::singleShot(5, this, &MainWindow::setScrollValue);
                // 接收已结束（AI 输出完毕）：全量刷新 setText() 已用保存的完整消息渲染。
                // 但重建前队列可能仍有积压（AI 输出速度大于渲染速度，thread 结束后未处理
                // 的文本仍在队列中）：重建完成后队列恢复的 recvMessage 仍需增量渲染补全，
                // 因此队列非空时恢复最后一条 AI 消息控件的接收指针（否则积压文本只累积
                // 到 message 不显示，最终消息缺尾部）
                if (!messageQueue.isEmpty() && !messageWidgetList.isEmpty()
                    && !messageWidgetList.last()->getIsUser()) {
                    messageRecvWidget = messageWidget;
                    itemRecvHLayout = itemHLayout;
                    itemRecvWidget = itemWidget;
                    recvItem = item;
                    isContinueShow = true;
                }
            } else {
                generateCurChatRecord(false, true);
                // 恢复或重建接收指针（本轮 AI 控件在重建后是否存在）：
                // - messageRecvWidget 非空：重建期间线程启动（started 信号在
                //   嵌套事件循环中派发）已由 messageStart 创建占位，保留
                // - 本轮接收控件已渲染过文本时，重建后列表末尾的 AI 控件即其
                //   重建版，可恢复接收指针
                // - 本轮占位文本为 null 时（空文本自动重试、响应首块未到、
                //   重新生成尚无输出）占位消息被保存过滤、重建后不存在：不能
                //   把接收指针指向列表末尾控件——它是用户消息或上一轮 AI 控件，
                //   本轮空回复收尾会把该控件 setText 清空并从聊天区删除（用户
                //   消息部件被误删），列表缺失还会使后续保存把数据库同步截断
                //   （DELETE seq >= diffIndex 后按新列表重插）。此时按
                //   messageStart 的占位创建流程重建接收占位控件，使空回复
                //   收尾（移除占位 + 自动重试）与流式增量渲染链路保持完整
                if (messageRecvWidget) {
                    // 已在重建期间就绪，不处理
                } else if (recvWidgetHasText && !messageWidgetList.isEmpty()
                           && !messageWidgetList.last()->getIsUser()) {
                    messageRecvWidget = messageWidget;
                    itemRecvHLayout = itemHLayout;
                    itemRecvWidget = itemWidget;
                    recvItem = item;
                    isContinueShow = true;
                } else {
                    messageStart();
                }
                // 重建期间到达的追加文本只累积在 message（isContinueShow=false 时不渲染）。
                // 线程完成信号到达时（onThreadFinished）messageFinish 不会提前执行：队列冻结
                // 期间 messageQueue 必然非空（或重建前已清空），故重建期间 isSending 保持 true，
                // 此处正常恢复接收指针；重建完成后队列恢复处理，待队列清空时由 recvMessage
                // 触发 messageFinish 统一收尾（补全尾部文本、移除 loading）。下方 !isSending
                // 分支为防御性兜底（若未来触发机制变化，重建期间接收已结束则补做收尾）
                if (!isSending && messageRecvWidget) {
                    if (messageRecvWidget->getText() != message) {
                        messageRecvWidget->setText(message);
                        updateItemLayout(itemRecvWidget, messageRecvWidget, recvItem);
                        ensureFrameStyle("regenerate");
                    }
                    if (!messageRecvWidget->getIsRemoveloadingWidget()) {
                        messageRecvWidget->removeLoadingWidget();
                        messageRecvWidget->updateFunWidgetSize(curDpi, initDpi);
                    }
                    if (messageRecvWidget && itemRecvWidget && itemRecvHLayout && recvItem) {
                        updateItemLayout(itemRecvWidget, messageRecvWidget, recvItem);
                    }
                    if (message.isEmpty() && !messageWidgetList.isEmpty()
                        && chatShow->count() > 0) {
                        // 同上：不能同步 delete —— 控件可能仍停留在渲染等待的嵌套
                        // 事件循环中，改用 abortRendering 接管销毁时机（摘除父级，
                        // 渲染栈退出后再销毁）
                        MessageWidget *lastMessageWidget = messageWidgetList.takeLast();
                        if (lastMessageWidget)
                            lastMessageWidget->abortRendering();
                        int last = chatShow->count() - 1;
                        QWidget *itemWidget = chatShow->itemWidget(chatShow->item(last));
                        if (itemWidget)
                            itemWidget->deleteLater();
                        QListWidgetItem *lastItem = chatShow->takeItem(last);
                        delete lastItem;
                        // 空文本自动重试（判定同 messageFinish 路径）：次数上限与
                        // "用户主动停止即结束"统一在 messageRenewResponse(isAutoRetry=true) 内处理
                        messageRenewResponse(true);
                    }
                }
            }
        }
    } while (isRegeneratePending && !hasPendingChatSwitch);
    isRegenerating = false;
    // 重建期间被推迟的切换请求（点击落在重建的嵌套事件循环内）：重建完成后执行。
    // 切换优先于继续重建：切换会重置接收链与当前会话，重建轮据此提前退出
    // （待重建标记由切换收尾重新调度，在切换后的列表上再重建一次）；
    // 切换内部会重置接收链与队列状态，下方队列恢复检查随之按新会话状态判断
    if (hasPendingChatSwitch)
        requestChatRecordSwitch(pendingChatSessionId, pendingChatSwitchIsNewChat);
    // 重建完成：恢复队列处理（重建期间 queueMessage 只入队、recvMessage 暂停，
    // 文本保留在队列中）。延迟到重建栈退出后执行：接收仍在进行（isSending）或
    // 接收已结束但队列仍有积压（AI 输出完毕时未渲染的尾部文本）时，逐条增量渲染
    // 补全到接收控件（!isSending 分支已在重建时恢复接收指针），避免与重建收尾的
    // 全量刷新 setText 交错；队列为空时无处理需求
    if (!isProcessing && !messageQueue.isEmpty()) {
        isProcessing = true;
        QTimer::singleShot(0, this, [this]() {
            // messageQueue.head() 在此延迟回调执行时才求值：重建完成置 isProcessing 后、
            // 本回调执行前，线程可能已触发 queueMessage 并经其同步处理链（2557 行）
            // 把积压文本全部消费完（dequeue 至队列空）。此时 head() 返回空值，
            // 不能将空文本传给 recvMessage；队列为空则还原处理标志，避免空渲染，
            // 也避免 isProcessing 残留为 true 导致后续队列处理链无法启动。
            // （队列收尾触发 messageFinish 已由实际处理最后一条文本的 recvMessage 分支负责，
            // 此处无需重复）
            if (!messageQueue.isEmpty()) {
                recvMessage(messageQueue.head());
            } else {
                isProcessing = false;
            }
        });
    }
}

void MainWindow::setScrollValue()
{
    int newMaxScrollValue = chatShow->verticalScrollBar()->maximum();
    if (maxScrollValue != 0) {
        int newValue =
                qRound(static_cast<qreal>(currentScrollValue) / maxScrollValue * newMaxScrollValue);
        chatShow->verticalScrollBar()->setValue(newValue);
    }
}

void MainWindow::showChatRecords()
{
    if (!chatRecordsWidgetIsOpen) {
        saveCurChatRecord();
        chatRecordsGenerateItem();
        chatRecordsWidget->raise();
        chatRecordsAnimationMove->setStartValue(chatRecordsWidget->geometry());
        chatRecordsAnimationMove->setEndValue(QRect(
                0, titleWidget->height(), chatRecordsWidget->width(), chatRecordsWidget->height()));
        chatRecordsAnimationMove->start();
        chatRecordsWidgetIsOpen = true;
    } else {
        if (settingWidgetIsOpen) {
            settingAnimationMove->setStartValue(settingWidget->geometry());
            settingAnimationMove->setEndValue(QRect(-settingWidget->width(), titleWidget->height(),
                                                    settingWidget->width(),
                                                    settingWidget->height()));
            settingAnimationMove->start();
            settingWidgetIsOpen = false;
        }
        chatRecordsAnimationMove->setStartValue(chatRecordsWidget->geometry());
        chatRecordsAnimationMove->setEndValue(
                QRect(-chatRecordsWidget->width(), titleWidget->height(),
                      chatRecordsWidget->width(), chatRecordsWidget->height()));
        chatRecordsAnimationMove->start();
        chatRecordsWidgetIsOpen = false;
    }
    pushButtonIsPress = true;
}

void MainWindow::showSearchRecords()
{
    QString text = chatRecordsWidget->getLineEditText();
    chatRecordsWidget->delAllListItems();
    chatRecordsGenerateItem(text);
}

void MainWindow::clearAllChatRecords()
{
    chatRecordsWidget->delAllListItems();
    if (chatRecordStore)
        chatRecordStore->deleteAllSessions();
}

void MainWindow::generateChatRecord(QListWidgetItem *item)
{
    // 点击瞬间取值：item 可能随记录列表刷新失效，后续只使用会话 id
    requestChatRecordSwitch(chatRecordsWidget->listItemToString(item).toInt(), false);
}

void MainWindow::newChat()
{
    // 新建聊天：带上重入保护，构建/重建期间点击同样只记录目标待执行
    requestChatRecordSwitch(-1, true);
}

void MainWindow::requestChatRecordSwitch(int sessionId, bool isNewChat)
{
    pendingChatSessionId = sessionId;
    pendingChatSwitchIsNewChat = isNewChat;
    hasPendingChatSwitch = true;

    // 已有切换流程或消息列表重建（窗口尺寸变化触发）在进行：本次点击只排队。
    // 构建消息列表期间会进入控件渲染的嵌套事件循环，快速连点即在此时重入：
    // 若立即执行，半成品列表会被保存回数据库（会话记录被截断），且正在构建的旧
    // 流程会把剩余消息追加到新列表里（显示与记录不符）
    if (isSwitchingChatRecord || isRegenerating) {
        qDebug() << "requestChatRecordSwitch pending:" << sessionId << isNewChat;
        return;
    }

    isSwitchingChatRecord = true;
    // 连续快速点击只保留最后一次目标：每轮先清待执行标记，本轮结束后若期间
    // 又收到点击则继续执行（同 messageWidgetRegenerate 的待重建处理）
    do {
        hasPendingChatSwitch = false;
        qDebug() << "applyChatRecordSwitch:" << pendingChatSessionId << pendingChatSwitchIsNewChat;
        applyChatRecordSwitch(pendingChatSessionId, pendingChatSwitchIsNewChat);
    } while (hasPendingChatSwitch);
    isSwitchingChatRecord = false;
    // 切换/构建期间被推迟的窗口重建请求（WM_EXITSIZEMOVE 等触发的 messageWidgetRegenerate）：
    // 此时消息列表已构建完成，延迟到事件循环空闲时执行（经统一调度入口合并去重）
    if (isRegeneratePending && !isRegenerating)
        scheduleMessageWidgetRegenerate();
}

void MainWindow::applyChatRecordSwitch(int sessionId, bool isNewChat)
{
    if (isSending) {
        thread->stop();
        isSending = false;
        // 主动停止线程：放弃本轮收尾，防止积压队列清空时误触发 messageFinish
        isThreadFinished = false;
        if (messageRecvWidget)
            messageRecvWidget->breakHandle();
    }
    saveCurChatRecord();
    // 旧消息控件即将被销毁：停止接收链并断开裸指针，防止流式渲染中的
    // setText 调用栈或排队的 recvMessage 回调在控件销毁后继续访问（悬空崩溃）
    resetRecvChain();
    messageWidgetList.clear();
    // 列表已清空：中止构建的半成品状态随之结束（清空后的保存本身无内容可写）。
    // 必须放在上方 saveCurChatRecord 之后：切换前的保存仍需拦截半成品列表
    isChatRecordBuildAbandoned = false;
    if (isNewChat)
        thinkTimeLengthList.clear();

    for (int i = 0; i < chatShow->count(); ++i) {
        QWidget *itemWidget = chatShow->itemWidget(chatShow->item(i));
        if (itemWidget)
            itemWidget->deleteLater();
    }
    chatShow->clear();
    // 切换聊天记录/新建聊天：恢复自动跟随，保证加载完成与后续追加都显示最新内容（底部）；
    // 新建时还避免上一会话中滚动查看历史关闭的自动跟随残留（清空时 value 本就为 0，
    // 不触发 valueChanged 恢复）导致新会话不滚动
    chatShow->resetScrollAutoChange();
    if (isNewChat) {
        curChatSessionId = -1;
        pushButtonIsPress = true;
    } else {
        curChatSessionId = sessionId;
        generateCurChatRecord();
    }
}
