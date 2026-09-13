#include "mainwindow.h"

#include <QtWebEngineCore/QWebEngineProfile>
#include <QQuickWindow>
#include <QDebug>
#include <QPointer>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      message(""),
      isShowFirst(true),
      isProcessing(false),
      isSetTexting(false),
      isRegenerating(false),
      isRegeneratePending(false),
      pushButtonIsPress(false),
      screenChanged(false),
      isSending(false),
      isThreadFinished(false),
      isContinueShow(true),
      isScreenMax(false),
      isScreenHalf(false),
      isChangeRectFirst(false),
      dragStartWidth(-1),
      dragStartHeight(-1),
      isSizeMoveDrag(false),
      dragRegenerateDone(false),
      pendingRegenerateAfterResize(false),
      isMinimizedState(false),
      lastRegenerateWidth(-1),
      lastRegenerateHeight(-1),
      isDpiChanged(false),
      avoidRepeatSelfFun(false),
      borderLen(3)
{
    appContext = new AppContext();
    QWebEngineProfile *profile = new QWebEngineProfile("shared");
    profile->setHttpCacheType(QWebEngineProfile::DiskHttpCache);
    profile->setCachePath(appContext->webEngineCacheDir());
    appContext->setWebEngineProfile(profile);
    QWebEngineView *dummyView = new QWebEngineView();
    // dummyView->setPage(new WebEnginePage(sharedProfile, dummyView));
    WebEnginePage *page = new WebEnginePage(appContext->webEngineProfile(), nullptr); // 无 parent
    dummyView->setPage(page);
    connect(
            dummyView, &QWebEngineView::loadFinished, dummyView,
            [dummyView]() {
                qDebug() << "load finished";
                dummyView->stop();
                QTimer::singleShot(0, dummyView, [dummyView]() {
                    if (dummyView->page()) {
                        dummyView->page()->triggerAction(QWebEnginePage::Stop);
                    }
                    QTimer::singleShot(100, dummyView, [dummyView]() {
                        QWebEnginePage *page = dummyView->page();
                        dummyView->setPage(nullptr);
                        if (page) {
                            delete page;
                        }
                        QTimer::singleShot(500, dummyView, [dummyView]() {
                            delete dummyView;
                            qDebug() << "cleanup done";
                        });
                    });
                });
            },
            Qt::QueuedConnection);

    // connect(
    //         dummyView, &QWebEngineView::loadFinished, dummyView,
    //         [dummyView, page]() {
    //             qDebug() << "load finished, scheduling safe delete";
    //             dummyView->stop();
    //             QTimer::singleShot(0, [dummyView, page]() {
    //                 dummyView->setPage(nullptr);
    //                 delete page;
    //                 delete dummyView;
    //                 qDebug() << "safe delete completed";
    //             });
    //         },
    //         Qt::QueuedConnection);

    // QTimer::singleShot(1000, dummyView, [dummyView]() {
    //     qDebug() << "delayed delete dummyView";
    //     QWebEnginePage *page = dummyView->page();
    //     dummyView->setPage(nullptr);
    //     delete page;
    //     dummyView->deleteLater();
    // });

    // connect(dummyView, &QWebEngineView::loadFinished, this, [dummyView]() {
    //     qDebug() << "delayed delete dummyView";
    //     QWebEnginePage *page = dummyView->page();
    //     dummyView->setPage(nullptr);
    //     delete page;
    //     dummyView->deleteLater();
    // });
    dummyView->load(QUrl("about:blank"));
    // connect(dummyView, &QWebEngineView::loadFinished, []() { qDebug() << "dummyView delete"; });

    setMinimumSize(1110, 795);
    resize(1200, 800);
    setWindowFlags(Qt::FramelessWindowHint | Qt::WindowSystemMenuHint);
    setMouseTracking(true);

    mouseLeftButtonIsPress = false;
    regionDir = RegionEnum::Middle;
    padding = 2;
    titleWidgetInit();
    chatFun = new FunWidget(appContext);
    // qDebug() << "FunWidget size:" << chatFun->size();
    chatFun->connectChatRecordsButtonClick(this, &MainWindow::showChatRecords);
    chatFun->connectNewChatButtonClick(this, &MainWindow::newChat);
    chatShow = new ListWidget();
    chatShowWidget = new Widget();
    chatShowWidget->setMinimumHeight(244);
    chatShowWidget->resize(1200, 500);
    chatShowWidget->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    chatShowVLayout = new QVBoxLayout(chatShowWidget);
    chatShowVLayout->addWidget(chatShow);
    chatShowVLayout->setContentsMargins(20, 4, 10, 16);
    chatInput = new TextEditFull(appContext);
    chatInput->connectSendButtonClick(this, &MainWindow::sendMessage);
    chatInputWidget = new Widget();
    chatInputWidget->setMinimumHeight(100);
    chatInputWidget->resize(1200, 200);
    chatInputWidget->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    chatInputVLayout = new QVBoxLayout(chatInputWidget);
    chatInputVLayout->addWidget(chatInput);
    chatInputVLayout->setContentsMargins(20, 0, 20, 20);
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
    emptyTextLabel->move((width() - emptyTextLabel->width()) / 2,
                         titleWidget->height() + chatFun->height() + chatShowWidget->height() + 10);
    emptyTextLabel->raise();
    emptyTextLabel->hide();
    textCopyLabel = new PrintLabel("文本复制成功", appContext, this);
    textCopyLabel->move((width() - textCopyLabel->width()) / 2,
                        titleWidget->height() + chatFun->height() + chatShowWidget->height() + 10);
    textCopyLabel->raise();
    textCopyLabel->hide();

    messageWidgetList.clear();
    lastScreen = curScreen = screen();
    initDpi = lastDpi = curDpi = curScreen->logicalDotsPerInch();
    thinkExpandedList.clear();
    thinkTimeLengthList.clear();
    lastNormalGeometry = geometry();
    uiRectWidth = width();
    uiRectHeight = height();
    screens = QApplication::screens();
    for (QScreen *screen : screens) {
        connect(screen, &QScreen::logicalDotsPerInchChanged, this, &MainWindow::onDpiChanged);
    }

    widgetSizeDict["MainWindow"] = size();
    widgetSizeDict["MainWindow minimumSize"] = minimumSize();
    widgetSizeDict["mainWidget"] = mainWidget->size();
    widgetSizeDict["mainWidget x"] = mainWidget->x();
    widgetSizeDict["mainWidget y"] = mainWidget->y();

    checkGraphicsBackend();

#ifdef Q_OS_WIN
    HWND hwnd = reinterpret_cast<HWND>(winId());
    DWORD style = GetWindowLong(hwnd, GWL_STYLE);
    SetWindowLongPtr(hwnd, GWL_STYLE, style | WS_OVERLAPPEDWINDOW);
    SetWindowPos(hwnd, NULL, 0, 0, 0, 0,
                 SWP_FRAMECHANGED | SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOOWNERZORDER);

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

MainWindow::~MainWindow() { }

void MainWindow::titleWidgetInit()
{
    titleWidget = new TitleWidget(appContext);
    connect(titleWidget, &TitleWidget::minimizeClicked, this, &MainWindow::uiMinimize);
    connect(titleWidget, &TitleWidget::maximizeClicked, this, &MainWindow::uiMaximize);
    connect(titleWidget, &TitleWidget::closeClicked, this, &MainWindow::uiClose);
}

void MainWindow::settingWidgetInit()
{
    settingWidget = new SettingWidget(appContext, mainWidget);
    settingWidget->setGeometry(-mainWidget->width() / 3, titleWidget->height(),
                               mainWidget->width() / 3,
                               mainWidget->height() - titleWidget->height());
    connect(settingWidget, &SettingWidget::baseUrlTextChanged, this,
            &MainWindow::onBaseUrlTextChanged);
    connect(settingWidget, &SettingWidget::apiKeyTextChanged, this,
            &MainWindow::onApiKeyTextChanged);
    connect(settingWidget, &SettingWidget::modelNameTextChanged, this,
            &MainWindow::onModelNameTextChanged);
    connect(settingWidget, &SettingWidget::maxTokensBoxValueChanged, this,
            &MainWindow::onMaxTokensBoxValueChanged);
    connect(settingWidget, &SettingWidget::topPBoxValueChanged, this,
            &MainWindow::onTopPBoxValueChanged);
    connect(settingWidget, &SettingWidget::temperatureBoxValueChanged, this,
            &MainWindow::onTemperatureBoxValueChanged);
    connect(settingWidget, &SettingWidget::maxTokensSliderValueChanged, this,
            &MainWindow::onMaxTokensSliderValueChanged);
    connect(settingWidget, &SettingWidget::topPSliderValueChanged, this,
            &MainWindow::onTopPSliderValueChanged);
    connect(settingWidget, &SettingWidget::temperatureSliderValueChanged, this,
            &MainWindow::onTemperatureSliderValueChanged);

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

    curChatFile = "";
    chatRecordsWidgetIsOpen = false;
}

void MainWindow::checkGraphicsBackend()
{
    qwindow = new QQuickWindow();
    qwindow->show();
    QTimer::singleShot(500, [&]() {
        qDebug() << "===== 图形后端信息 =====";
        qDebug() << "当前渲染器:" << qwindow->rendererInterface()->graphicsApi();
        qDebug() << "场景图后端:" << qwindow->sceneGraphBackend();
        qDebug() << "======================";
        qwindow->close();
        qwindow->deleteLater();
    });
}

void MainWindow::applyDWMShadow()
{
#ifdef Q_OS_WIN
    HWND hwnd = reinterpret_cast<HWND>(winId());
    if (!hwnd || !IsWindow(hwnd)) {
        return;
    }

    DWMNCRENDERINGPOLICY policy = DWMNCRP_ENABLED;
    DwmSetWindowAttribute(hwnd, DWMWA_NCRENDERING_POLICY, &policy, sizeof(policy));

    MARGINS margins = { 1, 1, 1, 1 };
    DwmExtendFrameIntoClientArea(hwnd, &margins);

    RedrawWindow(hwnd, NULL, NULL, RDW_FRAME | RDW_INVALIDATE | RDW_UPDATENOW);
    // SetWindowPos(hwnd, NULL, 0, 0, 0, 0,
    //              SWP_FRAMECHANGED | SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOOWNERZORDER
    //                      | SWP_NOACTIVATE | SWP_SHOWWINDOW);
#endif
}

void MainWindow::showEvent(QShowEvent *event)
{
    QMainWindow::showEvent(event);
    // 首次显示完成后，WM_SIZE 才允许触发聊天记录重建（避免启动阶段误触发）
    isShowFirst = false;

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
        *result = 0;
        return true;
    }

    case WM_NCHITTEST: {
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
                    *result = HTCLIENT;
                    return true;
                }
                *result = HTCAPTION;
                return true;
            }
            *result = HTCLIENT;
            return true;
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
            *result = HTTOPLEFT;
            return true;
        }
        if (nX >= w - detectBorder && nX < w && nY >= 0 && nY < detectBorder) {
            *result = HTTOPRIGHT;
            return true;
        }
        if (nX >= 0 && nX < detectBorder && nY >= h - detectBorder && nY < h) {
            *result = HTBOTTOMLEFT;
            return true;
        }
        if (nX >= w - detectBorder && nX < w && nY >= h - detectBorder && nY < h) {
            *result = HTBOTTOMRIGHT;
            return true;
        }

        if (nY >= 0 && nY < detectBorder && nX >= detectBorder && nX < w - detectBorder) {
            *result = HTTOP;
            return true;
        }
        if (nY >= h - detectBorder && nY < h && nX >= detectBorder && nX < w - detectBorder) {
            *result = HTBOTTOM;
            return true;
        }
        if (nX >= 0 && nX < detectBorder && nY >= detectBorder && nY < h - detectBorder) {
            *result = HTLEFT;
            return true;
        }
        if (nX >= w - detectBorder && nX < w && nY >= detectBorder && nY < h - detectBorder) {
            *result = HTRIGHT;
            return true;
        }

        if (nY >= detectBorder && nY < titleHeight) {
            if (nX >= btnLeft && nX < btnRight) {
                *result = HTCLIENT;
                return true;
            }
            if (nX >= detectBorder && nX < btnLeft) {
                // qDebug() << "MainWindow::nativeEvent HTCAPTION";
                *result = HTCAPTION;
                return true;
            }
        }

        *result = HTCLIENT;
        return true;
    }

    case WM_GETMINMAXINFO: {
        MINMAXINFO *mmi = reinterpret_cast<MINMAXINFO *>(msg->lParam);
        MONITORINFO mi = { sizeof(mi) };
        if (GetMonitorInfo(MonitorFromWindow(hwnd, MONITOR_DEFAULTTONEAREST), &mi)) {
            mmi->ptMaxPosition.x = mi.rcWork.left + 1;
            mmi->ptMaxPosition.y = mi.rcWork.top + 1;
            mmi->ptMaxSize.x = mi.rcWork.right - mi.rcWork.left;
            mmi->ptMaxSize.y = mi.rcWork.bottom - mi.rcWork.top;
        }
        mmi->ptMinTrackSize.x = 1100;
        mmi->ptMinTrackSize.y = 795;
        *result = 0;
        return true;
    }

    case WM_SIZE: {
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
                    QTimer::singleShot(0, this, &MainWindow::messageWidgetRegenerate);
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
        // 区分“仅移动”与“调整大小”（仅移动不应触发聊天记录重建）
        isSizeMoveDrag = true;
        dragRegenerateDone = false;
        RECT rect;
        GetWindowRect(hwnd, &rect);
        dragStartWidth = rect.right - rect.left;
        dragStartHeight = rect.bottom - rect.top;
        break;
    }

    case WM_MOVING: {
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

    case WM_EXITSIZEMOVE: {
        QTimer::singleShot(10, this, &MainWindow::applyDWMShadow);
        isSizeMoveDrag = false;

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
                QTimer::singleShot(0, this, &MainWindow::messageWidgetRegenerate);
            } else {
                lastRegenerateHeight = h;
                qDebug() << "WM_EXITSIZEMOVE pending canceled, width back to start";
            }
        } else if (dragRegenerateDone) {
            // 拖动过程中已标记过；若最终宽度与重建时不同（继续拉伸）则补一次
            if (w != lastRegenerateWidth) {
                lastRegenerateWidth = w;
                lastRegenerateHeight = h;
                qDebug() << "WM_EXITSIZEMOVE width still changed, regenerate" << w << h;
                QTimer::singleShot(0, this, &MainWindow::messageWidgetRegenerate);
            } else {
                lastRegenerateHeight = h;
                qDebug() << "WM_EXITSIZEMOVE already regenerated, skip";
            }
        } else if (w != dragStartWidth) {
            // 拖动中未触发（如 Snap 宽度变化发生在最后时刻）：现在重建
            lastRegenerateWidth = w;
            lastRegenerateHeight = h;
            qDebug() << "WM_EXITSIZEMOVE width changed, regenerate" << w << h;
            QTimer::singleShot(0, this, &MainWindow::messageWidgetRegenerate);
        } else {
            lastRegenerateHeight = h;
            qDebug() << "WM_EXITSIZEMOVE move or height only, skip regenerate";
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
    }
    QMainWindow::moveEvent(event);
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

void MainWindow::regionDivision()
{
    if ((cursorGlobalX >= uiGlobalTL.x()) && (cursorGlobalX <= uiGlobalTL.x() + padding)
        && (cursorGlobalY >= uiGlobalTL.y()) && (cursorGlobalY <= uiGlobalTL.y() + padding)) {
        regionDir = RegionEnum::LeftTop;
        setCursor(QCursor(Qt::SizeFDiagCursor));
    } else if ((cursorGlobalX >= uiGlobalBR.x() - padding) && (cursorGlobalX <= uiGlobalBR.x())
               && (cursorGlobalY >= uiGlobalTL.y())
               && (cursorGlobalY <= uiGlobalTL.y() + padding)) {
        regionDir = RegionEnum::RightTop;
        setCursor(QCursor(Qt::SizeBDiagCursor));
    } else if ((cursorGlobalX >= uiGlobalTL.x()) && (cursorGlobalX <= uiGlobalTL.x() + padding)
               && (cursorGlobalY >= uiGlobalBR.y() - padding)
               && (cursorGlobalY <= uiGlobalBR.y())) {
        regionDir = RegionEnum::LeftBottom;
        setCursor(QCursor(Qt::SizeBDiagCursor));
    } else if ((cursorGlobalX >= uiGlobalBR.x() - padding) && (cursorGlobalX <= uiGlobalBR.x())
               && (cursorGlobalY >= uiGlobalBR.y() - padding)
               && (cursorGlobalY <= uiGlobalBR.y())) {
        regionDir = RegionEnum::RightBottom;
        setCursor(QCursor(Qt::SizeFDiagCursor));
    } else if ((cursorGlobalX >= uiGlobalTL.x()) && (cursorGlobalX <= uiGlobalTL.x() + padding)) {
        regionDir = RegionEnum::Left;
        setCursor(QCursor(Qt::SizeHorCursor));
    } else if ((cursorGlobalX >= uiGlobalBR.x() - padding) && (cursorGlobalX <= uiGlobalBR.x())) {
        regionDir = RegionEnum::Right;
        setCursor(QCursor(Qt::SizeHorCursor));
    } else if ((cursorGlobalY >= uiGlobalTL.y()) && (cursorGlobalY <= uiGlobalTL.y() + padding)) {
        regionDir = RegionEnum::Top;
        setCursor(QCursor(Qt::SizeVerCursor));
    } else if ((cursorGlobalY >= uiGlobalBR.y() - padding) && (cursorGlobalY <= uiGlobalBR.y())) {
        regionDir = RegionEnum::Bottom;
        setCursor(QCursor(Qt::SizeVerCursor));
    } else if ((cursorGlobalX >= uiGlobalTL.x() + padding + 1)
               && (cursorGlobalX <= uiGlobalBR.x() - padding - 1)
               && (cursorGlobalY >= uiGlobalTL.y() + padding + 1)
               && (cursorGlobalY <= uiGlobalTL.y() + titleWidget->height())) {
        if (cursorGlobalX <= uiGlobalBR.x() - titleWidget->getMinButtonSize().width()
                    - titleWidget->getMaxButtonSize().width()
                    - titleWidget->getCloseButtonSize().width() - 1)
            regionDir = RegionEnum::Title;
        else
            regionDir = RegionEnum::Button;
        setCursor(QCursor(Qt::ArrowCursor));
    } else {
        regionDir = RegionEnum::Middle;
        setCursor(QCursor(Qt::ArrowCursor));
    }
}

void MainWindow::UiStretch()
{
    QRect uiGlobalRect(uiGlobalTL, uiGlobalBR);
    switch (regionDir) {
    case RegionEnum::Left:
        if (uiGlobalBR.x() - cursorGlobalX
            > minimumWidth() - 2 * widgetSizeDict["mainWidget x"].value<int>())
            uiGlobalRect.setX(cursorGlobalX);
        else
            uiGlobalRect.setX(uiGlobalBR.x()
                              - (minimumWidth() - 2 * widgetSizeDict["mainWidget x"].value<int>())
                              + 1);
        break;
    case RegionEnum::Right:
        if (cursorGlobalX - uiGlobalTL.x()
            > minimumWidth() - 2 * widgetSizeDict["mainWidget x"].value<int>())
            uiGlobalRect.setWidth(cursorGlobalX - uiGlobalTL.x());
        else
            uiGlobalRect.setWidth(minimumWidth() - 2 * widgetSizeDict["mainWidget x"].value<int>());
        break;
    case RegionEnum::Top:
        if (uiGlobalBR.y() - cursorGlobalY
            > minimumHeight() - 2 * widgetSizeDict["mainWidget y"].value<int>())
            uiGlobalRect.setY(cursorGlobalY);
        else
            uiGlobalRect.setY(uiGlobalBR.y()
                              - (minimumHeight() - 2 * widgetSizeDict["mainWidget y"].value<int>())
                              + 1);
        break;
    case RegionEnum::Bottom:
        if (cursorGlobalY - uiGlobalTL.y()
            > minimumHeight() - 2 * widgetSizeDict["mainWidget y"].value<int>())
            uiGlobalRect.setHeight(cursorGlobalY - uiGlobalTL.y());
        else
            uiGlobalRect.setHeight(minimumHeight()
                                   - 2 * widgetSizeDict["mainWidget y"].value<int>());
        break;
    case RegionEnum::LeftTop:
        if (uiGlobalBR.x() - cursorGlobalX
            > minimumWidth() - 2 * widgetSizeDict["mainWidget x"].value<int>())
            uiGlobalRect.setX(cursorGlobalX);
        else
            uiGlobalRect.setX(uiGlobalBR.x()
                              - (minimumWidth() - 2 * widgetSizeDict["mainWidget x"].value<int>())
                              + 1);
        if (uiGlobalBR.y() - cursorGlobalY
            > minimumHeight() - 2 * widgetSizeDict["mainWidget y"].value<int>())
            uiGlobalRect.setY(cursorGlobalY);
        else
            uiGlobalRect.setY(uiGlobalBR.y()
                              - (minimumHeight() - 2 * widgetSizeDict["mainWidget y"].value<int>())
                              + 1);
        break;
    case RegionEnum::RightTop:
        if (cursorGlobalX - uiGlobalTL.x()
            > minimumWidth() - 2 * widgetSizeDict["mainWidget x"].value<int>())
            uiGlobalRect.setWidth(cursorGlobalX - uiGlobalTL.x());
        else
            uiGlobalRect.setWidth(minimumWidth() - 2 * widgetSizeDict["mainWidget x"].value<int>());
        if (uiGlobalBR.y() - cursorGlobalY
            > minimumHeight() - 2 * widgetSizeDict["mainWidget y"].value<int>())
            uiGlobalRect.setY(cursorGlobalY);
        else
            uiGlobalRect.setY(uiGlobalBR.y()
                              - (minimumHeight() - 2 * widgetSizeDict["mainWidget y"].value<int>())
                              + 1);
        break;
    case RegionEnum::LeftBottom:
        if (uiGlobalBR.x() - cursorGlobalX
            > minimumWidth() - 2 * widgetSizeDict["mainWidget x"].value<int>())
            uiGlobalRect.setX(cursorGlobalX);
        else
            uiGlobalRect.setX(uiGlobalBR.x()
                              - (minimumWidth() - 2 * widgetSizeDict["mainWidget x"].value<int>())
                              + 1);
        if (cursorGlobalY - uiGlobalTL.y()
            > minimumHeight() - 2 * widgetSizeDict["mainWidget y"].value<int>())
            uiGlobalRect.setHeight(cursorGlobalY - uiGlobalTL.y());
        else
            uiGlobalRect.setHeight(minimumHeight()
                                   - 2 * widgetSizeDict["mainWidget y"].value<int>());
        break;
    case RegionEnum::RightBottom:
        if (cursorGlobalX - uiGlobalTL.x()
            > minimumWidth() - 2 * widgetSizeDict["mainWidget x"].value<int>())
            uiGlobalRect.setWidth(cursorGlobalX - uiGlobalTL.x());
        else
            uiGlobalRect.setWidth(minimumWidth() - 2 * widgetSizeDict["mainWidget x"].value<int>());
        if (cursorGlobalY - uiGlobalTL.y()
            > minimumHeight() - 2 * widgetSizeDict["mainWidget y"].value<int>())
            uiGlobalRect.setHeight(cursorGlobalY - uiGlobalTL.y());
        else
            uiGlobalRect.setHeight(minimumHeight()
                                   - 2 * widgetSizeDict["mainWidget y"].value<int>());
    default:
        break;
    }
    QRect windowGlobalRect(uiGlobalRect.x() - widgetSizeDict["mainWidget x"].value<int>(),
                           uiGlobalRect.y() - widgetSizeDict["mainWidget y"].value<int>(),
                           uiGlobalRect.width() + 2 * widgetSizeDict["mainWidget x"].value<int>(),
                           uiGlobalRect.height() + 2 * widgetSizeDict["mainWidget y"].value<int>());
    setGeometry(windowGlobalRect);
}

void MainWindow::UiDrag(QPoint globalPos)
{
    move(pressPosDistanceUiGlobalTL + globalPos);
}

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
        mouseLeftButtonIsPress = false;
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
            curDpi = curScreen->logicalDotsPerInch();
            appContext->setWindowFontPixelSize(
                    std::ceil(appContext->windowFontPointSize() * (curDpi / 72)));
            appContext->setTitleFontPixelSize(
                    std::ceil(appContext->titleFontPointSize() * (curDpi / 72)));
            screenChanged = false;
        }
    }
    QMainWindow::mouseReleaseEvent(event);
}

void MainWindow::onDpiChanged()
{
    qDebug() << "onDpiChanged";
    lastDpi = curDpi;
    curDpi = curScreen->logicalDotsPerInch();
    appContext->setWindowFontPixelSize(
            std::ceil(appContext->windowFontPointSize() * (curDpi / 72)));
    isDpiChanged = true;
}

void MainWindow::resizeEvent(QResizeEvent *event)
{
    Q_UNUSED(event);
    if (avoidRepeatSelfFun) {
        avoidRepeatSelfFun = false;
        return;
    }
    if (isDpiChanged) {
        setMinimumSize(qRound(widgetSizeDict["MainWindow minimumSize"].value<QSize>().width()
                              * curDpi / lastDpi),
                       qRound(widgetSizeDict["MainWindow minimumSize"].value<QSize>().height()
                              * curDpi / lastDpi));
        widgetSizeDict["MainWindow minimumSize"] = minimumSize();
    }
    // qDebug() << "resizeEvent isScreenMax" << isScreenMax;
    if (isScreenMax)
        mainWidget->setGeometry(0, 0, width(), height());
    else {
        if (isDpiChanged) {
            mainWidget->setGeometry(
                    qRound(widgetSizeDict["mainWidget x"].value<int>() * curDpi / lastDpi),
                    qRound(widgetSizeDict["mainWidget y"].value<int>() * curDpi / lastDpi),
                    qRound(widgetSizeDict["mainWidget"].value<QSize>().width() * curDpi / lastDpi),
                    qRound(widgetSizeDict["mainWidget"].value<QSize>().height() * curDpi
                           / lastDpi));
            widgetSizeDict["mainWidget"] = mainWidget->size();
            widgetSizeDict["mainWidget x"] = mainWidget->x();
            widgetSizeDict["mainWidget y"] = mainWidget->y();
        } else {
            qDebug() << "mainWidget geometry" << mainWidget->geometry();
            mainWidget->setGeometry(widgetSizeDict["mainWidget x"].value<int>(),
                                    widgetSizeDict["mainWidget y"].value<int>(),
                                    width() - 2 * widgetSizeDict["mainWidget x"].value<int>(),
                                    height() - 2 * widgetSizeDict["mainWidget y"].value<int>());
            qDebug() << "mainWidget x y" << widgetSizeDict["mainWidget x"].value<int>()
                     << widgetSizeDict["mainWidget y"].value<int>();
            qDebug() << "mainWidget geometry" << mainWidget->geometry();
            widgetSizeDict["mainWidget"] = mainWidget->size();
        }
    }
    if (isDpiChanged) {
        emptyTextLabel->updateSize(curDpi, lastDpi);
        textCopyLabel->updateSize(curDpi, lastDpi);
        settingWidget->updateSize(curDpi, lastDpi);
        titleWidget->updateSize(curDpi, lastDpi);
        chatRecordsWidget->updateSize(curDpi, lastDpi);
        chatFun->updateSize(curDpi, lastDpi);
        chatInput->updateSendButtonSize(curDpi, lastDpi);
    }
    settingWidget->resize(mainWidget->width() / 3, mainWidget->height() - titleWidget->height());
    chatRecordsWidget->resize(mainWidget->width() / 3,
                              mainWidget->height() - titleWidget->height());
    chatRecordsWidget->resetWidgetSize();
    if (settingWidgetIsOpen || chatRecordsWidgetIsOpen) {
        // 展开态内容区宽度统一使用 mainWidget->width() - mainWidget->width() / 3：
        // 侧栏宽度为 mainWidget->width() / 3，chatRecordsUiAnimationMove 的终态与
        // chatShowWidth(true) 都是该表达式；若这里改用 * 2 / 3，当窗口宽度除 3 余 2 时
        // 会比其小 1px，展开后拖拉窗口将使 AI 最大宽度的右边缘与用户 ImageLabel
        // 的右边缘错开 1px
        const int contentWidth = mainWidget->width() - mainWidget->width() / 3;
        chatFun->setFixedSize(contentWidth, chatFun->height());
        chatFun->resetWidgetSize();
        chatShow->resize(contentWidth - 30, chatShow->height());
        chatShowWidget->resize(contentWidth, chatShowWidget->height());
        chatInput->resize(contentWidth - 40, chatInput->height());
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
        chatFun->resetWidgetSize();
        // qDebug() << "mainWindow size:" << size();
        chatInput->resetWidgetSize();
        settingWidget->move(-settingWidget->width(), titleWidget->height());
        chatRecordsWidget->move(-chatRecordsWidget->width(), titleWidget->height());
    }
    if (isDpiChanged)
        settingWidget->updateLayoutMarginsSpacing(curDpi, lastDpi);
    settingWidget->resetWidgetSize();
    titleWidget->saveWidgetSize();
    emptyTextLabel->move((width() - emptyTextLabel->width()) / 2,
                         titleWidget->height() + chatFun->height() + chatShowWidget->height() + 10);
    textCopyLabel->move((width() - textCopyLabel->width()) / 2,
                        titleWidget->height() + chatFun->height() + chatShowWidget->height() + 10);
    if (isDpiChanged) {
        isDpiChanged = false;
        if ((qRound(widgetSizeDict["MainWindow"].value<QSize>().width() * curDpi / lastDpi)
             != width())
            || (qRound(widgetSizeDict["MainWindow"].value<QSize>().height() * curDpi / lastDpi)
                != height())) {
            avoidRepeatSelfFun = true;
            resize(qRound(widgetSizeDict["MainWindow"].value<QSize>().width() * curDpi / lastDpi),
                   qRound(widgetSizeDict["MainWindow"].value<QSize>().height() * curDpi / lastDpi));
        }
        messageWidgetRegenerate();
    }
    widgetSizeDict["MainWindow"] = size();
    qDebug() << "resizeEvent MainWindow size:" << size();
    qDebug() << "resizeEvent lastNormalGeometry:" << lastNormalGeometry;
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
    chatShow->resize(mainWidget->width() - rect.x() - chatRecordsWidget->width() - 30,
                     chatShow->height());
    chatShowWidget->resize(mainWidget->width() - rect.x() - chatRecordsWidget->width(),
                           chatShowWidget->height());
    chatInput->resize(mainWidget->width() - rect.x() - chatRecordsWidget->width() - 40,
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

void MainWindow::onBaseUrlTextChanged(const QString &text)
{
    try {
        QFile file(appContext->configFilePath());
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            throw std::runtime_error("无法打开文件");
        }

        QTextStream in(&file);
        QStringList lines;
        while (!in.atEnd()) {
            lines.append(in.readLine());
        }
        file.close();

        if (lines.isEmpty()) {
            lines.append("");
        }
        lines[0] = text;

        if (!file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate)) {
            throw std::runtime_error("无法写入文件");
        }

        QTextStream out(&file);
        for (int i = 0; i < lines.size(); ++i) {
            out << lines[i];
            if (i < lines.size() - 1) {
                out << "\n";
            }
        }
        file.close();

    } catch (const std::exception &e) {
        qDebug() << "错误：" << e.what();
    } catch (...) {
        qDebug() << "发生未知错误";
    }
}

void MainWindow::onApiKeyTextChanged(const QString &text)
{
    try {
        QFile file(appContext->configFilePath());
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            throw std::runtime_error("无法打开文件");
        }

        QTextStream in(&file);
        QStringList lines;
        while (!in.atEnd()) {
            lines.append(in.readLine());
        }
        file.close();

        while (lines.size() < 2) {
            lines.append("");
        }
        lines[1] = text;

        if (!file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate)) {
            throw std::runtime_error("无法写入文件");
        }

        QTextStream out(&file);
        for (int i = 0; i < lines.size(); ++i) {
            out << lines[i];
            if (i < lines.size() - 1) {
                out << "\n";
            }
        }
        file.close();

    } catch (const std::exception &e) {
        qDebug() << "错误：" << e.what();
    } catch (...) {
        qDebug() << "发生未知错误";
    }
}

void MainWindow::onModelNameTextChanged(const QString &text)
{
    try {
        QFile file(appContext->configFilePath());
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            throw std::runtime_error("无法打开文件");
        }

        QTextStream in(&file);
        QStringList lines;
        while (!in.atEnd()) {
            lines.append(in.readLine());
        }
        file.close();

        while (lines.size() < 3) {
            lines.append("");
        }
        lines[2] = text;

        if (!file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate)) {
            throw std::runtime_error("无法写入文件");
        }

        QTextStream out(&file);
        for (int i = 0; i < lines.size(); ++i) {
            out << lines[i];
            if (i < lines.size() - 1) {
                out << "\n";
            }
        }
        file.close();

    } catch (const std::exception &e) {
        qDebug() << "错误：" << e.what();
    } catch (...) {
        qDebug() << "发生未知错误";
    }
}

void MainWindow::onMaxTokensBoxValueChanged(int i)
{
    try {
        QFile file(appContext->configFilePath());
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            throw std::runtime_error("无法打开文件");
        }

        QTextStream in(&file);
        QStringList lines;
        while (!in.atEnd()) {
            lines.append(in.readLine());
        }
        file.close();

        while (lines.size() < 4) {
            lines.append("");
        }
        lines[3] = QString::number(i);

        if (!file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate)) {
            throw std::runtime_error("无法写入文件");
        }

        QTextStream out(&file);
        for (int i = 0; i < lines.size(); ++i) {
            out << lines[i];
            if (i < lines.size() - 1) {
                out << "\n";
            }
        }
        file.close();

    } catch (const std::exception &e) {
        qDebug() << "错误：" << e.what();
    } catch (...) {
        qDebug() << "发生未知错误";
    }

    settingWidget->maxTokensSliderSetValue(i);
}

void MainWindow::onTopPBoxValueChanged(double d)
{
    try {
        QFile file(appContext->configFilePath());
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            throw std::runtime_error("无法打开文件");
        }

        QTextStream in(&file);
        QStringList lines;
        while (!in.atEnd()) {
            lines.append(in.readLine());
        }
        file.close();

        while (lines.size() < 5) {
            lines.append("");
        }
        lines[4] = QString::number(d);

        if (!file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate)) {
            throw std::runtime_error("无法写入文件");
        }

        QTextStream out(&file);
        for (int i = 0; i < lines.size(); ++i) {
            out << lines[i];
            if (i < lines.size() - 1) {
                out << "\n";
            }
        }
        file.close();

    } catch (const std::exception &e) {
        qDebug() << "错误：" << e.what();
    } catch (...) {
        qDebug() << "发生未知错误";
    }

    settingWidget->topPSliderSetValue(static_cast<int>(d * 100));
}

void MainWindow::onTemperatureBoxValueChanged(double d)
{
    try {
        QFile file(appContext->configFilePath());
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            throw std::runtime_error("无法打开文件");
        }

        QTextStream in(&file);
        QStringList lines;
        while (!in.atEnd()) {
            lines.append(in.readLine());
        }
        file.close();

        while (lines.size() < 6) {
            lines.append("");
        }
        lines[5] = QString::number(d);

        if (!file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate)) {
            throw std::runtime_error("无法写入文件");
        }

        QTextStream out(&file);
        for (int i = 0; i < lines.size(); ++i) {
            out << lines[i];
            if (i < lines.size() - 1) {
                out << "\n";
            }
        }
        file.close();

    } catch (const std::exception &e) {
        qDebug() << "错误：" << e.what();
    } catch (...) {
        qDebug() << "发生未知错误";
    }

    settingWidget->temperatureSliderSetValue(static_cast<int>((d - 0.01) * 100));
}

void MainWindow::onMaxTokensSliderValueChanged(int i)
{
    try {
        QFile file(appContext->configFilePath());
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            throw std::runtime_error("无法打开文件");
        }

        QTextStream in(&file);
        QStringList lines;
        while (!in.atEnd()) {
            lines.append(in.readLine());
        }
        file.close();

        while (lines.size() < 4) {
            lines.append("");
        }
        lines[3] = QString::number(i);

        if (!file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate)) {
            throw std::runtime_error("无法写入文件");
        }

        QTextStream out(&file);
        for (int i = 0; i < lines.size(); ++i) {
            out << lines[i];
            if (i < lines.size() - 1) {
                out << "\n";
            }
        }
        file.close();

    } catch (const std::exception &e) {
        qDebug() << "错误：" << e.what();
    } catch (...) {
        qDebug() << "发生未知错误";
    }

    settingWidget->maxTokensBoxSetValue(i);
}

void MainWindow::onTopPSliderValueChanged(int i)
{
    try {
        QFile file(appContext->configFilePath());
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            throw std::runtime_error("无法打开文件");
        }

        QTextStream in(&file);
        QStringList lines;
        while (!in.atEnd()) {
            lines.append(in.readLine());
        }
        file.close();

        while (lines.size() < 5) {
            lines.append("");
        }
        lines[4] = QString::number(i / 100.0);

        if (!file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate)) {
            throw std::runtime_error("无法写入文件");
        }

        QTextStream out(&file);
        for (int i = 0; i < lines.size(); ++i) {
            out << lines[i];
            if (i < lines.size() - 1) {
                out << "\n";
            }
        }
        file.close();

    } catch (const std::exception &e) {
        qDebug() << "错误：" << e.what();
    } catch (...) {
        qDebug() << "发生未知错误";
    }

    settingWidget->topPBoxSetValue(i / 100.0);
}

void MainWindow::onTemperatureSliderValueChanged(int i)
{
    try {
        QFile file(appContext->configFilePath());
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            throw std::runtime_error("无法打开文件");
        }

        QTextStream in(&file);
        QStringList lines;
        while (!in.atEnd()) {
            lines.append(in.readLine());
        }
        file.close();

        while (lines.size() < 6) {
            lines.append("");
        }
        lines[5] = QString::number(i / 100.0 + 0.01);

        if (!file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate)) {
            throw std::runtime_error("无法写入文件");
        }

        QTextStream out(&file);
        for (int i = 0; i < lines.size(); ++i) {
            out << lines[i];
            if (i < lines.size() - 1) {
                out << "\n";
            }
        }
        file.close();

    } catch (const std::exception &e) {
        qDebug() << "错误：" << e.what();
    } catch (...) {
        qDebug() << "发生未知错误";
    }

    settingWidget->temperatureBoxSetValue(i / 100.0 + 0.01);
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

int MainWindow::messageWidgetTextMaxWidth() const
{
    // MessageWidget 构造时会在传入的最大宽度上再减去 10（内部边距），此处回加，
    // 使 MessageWidget 的实际最大宽度等于 messageWidgetMaxWidth()
    return messageWidgetMaxWidth() + messageWidgetTextMaxWidthExtra;
}

void MainWindow::updateItemLayout(QWidget *itemWidget, MessageWidget *messageWidget,
                                  QListWidgetItem *item)
{
    if (!itemWidget || !messageWidget)
        return;
    const int w = chatShow->width();
    const int h = messageWidget->height() + 10;
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

void MainWindow::messageWidgetResize(MessageWidget *selfMessageWidget)
{
    qDebug() << "messageWidgetResize start";
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
                    messageWidgetTextMaxWidth());
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

            thread = new MessageThread(text, context);
            chatInput->clearText();
            chatInput->setSending(true);
            isSending = true;
            isThreadFinished = false;
            qDebug() << "sendMessage:" << text;
        } else {
            //         emptyTextLabel->printStart();
        }
    } else {
        if (thread)
            thread->stop();
        isSending = false;
        // 主动停止线程：放弃本轮收尾，防止积压队列清空时误触发 messageFinish
        isThreadFinished = false;
        if (messageRecvWidget)
            messageRecvWidget->breakHandle();
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
    message.clear();
    // 新接收开始：会话切换时 resetRecvChain 置 false 的渲染开关在此恢复，
    // 否则切换后新发送的流式文本不会渲染
    isContinueShow = true;
    int i = messageWidgetList.size() - 1;
    if (i != 0) {
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
            messageWidgetList.size(), false, true, messageWidgetTextMaxWidth());
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

    HWND hwnd = reinterpret_cast<HWND>(winId());
    DWORD style = GetWindowLong(hwnd, GWL_STYLE);
    SetWindowLongPtr(hwnd, GWL_STYLE, style | WS_OVERLAPPEDWINDOW);
    SetWindowPos(hwnd, NULL, 0, 0, 0, 0,
                 SWP_FRAMECHANGED | SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOOWNERZORDER);
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

    HWND hwnd = reinterpret_cast<HWND>(winId());
    DWORD style = GetWindowLong(hwnd, GWL_STYLE);
    SetWindowLongPtr(hwnd, GWL_STYLE, style | WS_OVERLAPPEDWINDOW);
    SetWindowPos(hwnd, NULL, 0, 0, 0, 0,
                 SWP_FRAMECHANGED | SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOOWNERZORDER);

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
    // 接收真正结束（本函数由线程 finished 信号触发，是补全缺失文本的合适时机）：
    // 重建期间到达的追加文本只累积在 message 未渲染，重建完成后控件会缺少尾部文本，
    // 此处统一补全；此时接收已停止，不会与增量 setText 交错
    if (messageRecvWidget && isContinueShow && messageRecvWidget->getText() != message) {
        messageRecvWidget->setText(message);
        updateItemLayout(itemRecvWidget, messageRecvWidget, recvItem);
        HWND hwnd = reinterpret_cast<HWND>(winId());
        DWORD style = GetWindowLong(hwnd, GWL_STYLE);
        SetWindowLongPtr(hwnd, GWL_STYLE, style | WS_OVERLAPPEDWINDOW);
        SetWindowPos(hwnd, NULL, 0, 0, 0, 0,
                     SWP_FRAMECHANGED | SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOOWNERZORDER);
    }

    if (messageRecvWidget) {
        messageRecvWidget->removeLoadingWidget();
        messageRecvWidget->updateFunWidgetSize(curDpi, initDpi);
        // messageRecvWidget->toggleWidget();
    }

    if (messageRecvWidget && itemRecvWidget && itemRecvHLayout && recvItem) {
        updateItemLayout(itemRecvWidget, messageRecvWidget, recvItem);
    }

    // 空回复置占位移除：仅在接收控件与本轮 item 上有效。会话切换后这些指针已置空
    // （resetRecvChain），此时 message 必然被清空，若不加限制会误删新会话的最后一条消息
    if (message.isEmpty() && messageRecvWidget && itemRecvWidget && recvItem
        && !messageWidgetList.isEmpty() && chatShow->count() > 0) {
        delete messageWidgetList.takeLast();
        int last = chatShow->count() - 1;
        QWidget *itemWidget = chatShow->itemWidget(chatShow->item(last));
        if (itemWidget)
            itemWidget->deleteLater();
        QListWidgetItem *lastItem = chatShow->takeItem(last);
        delete lastItem;
        messageRenewResponse();
    }
    chatInput->setSending(false);
    isSending = false;

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
    messageQueue.clear();
    isProcessing = false;
    isContinueShow = false;
    isThreadFinished = false;
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

void MainWindow::textCopy() { }

void MainWindow::messageRenewResponse() { }

void MainWindow::writeToChatRecordFile(bool withholdCurChatFile)
{
    chatRecordFileName = "chat_";
    chatRecordFileName += QDateTime::currentDateTime().toString("yyyy_MM_dd_HH_mm_ss");
    chatRecordFileName += ".txt";

    QString filePath = QDir(appContext->chatRecordsDir()).filePath(chatRecordFileName);
    qDebug() << "writeToChatRecordFile filePath:" << filePath;

    try {
        QFile file(filePath);
        if (!file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Append)) {
            throw std::runtime_error("无法打开文件进行写入");
        }
        qDebug() << "writeToChatRecordFile success open file:";

        QTextStream out(&file);
        out.setEncoding(QStringConverter::Utf8);

        for (int i = 0; i < messageWidgetList.size(); i++) {
            MessageWidget *messageWidget = messageWidgetList.at(i);
            if (!messageWidget)
                continue;
            QString chatRecordStr = messageWidget->getText() + '\n'
                    + QString("消息部件思考时长:%1秒\n")
                              .arg(i < thinkTimeLengthList.size() ? thinkTimeLengthList.at(i) : 0)
                    + (messageWidget->getIsUser() ? "True\n" : "False\n");
            out << chatRecordStr;
        }

        file.close();

    } catch (const std::exception &e) {
        qDebug() << "发生未知错误：" << e.what();
    }

    if (!withholdCurChatFile) {
        curChatFile = chatRecordFileName;
    }
}

void MainWindow::saveCurChatRecord(bool withholdCurChatFile)
{
    if (messageWidgetList.size() != 0) {
        if (curChatFile.isEmpty()) {
            writeToChatRecordFile(withholdCurChatFile);
        } else {
            QString filePath = QDir(appContext->chatRecordsDir()).filePath(curChatFile);
            if (!QFile::exists(filePath)) {
                writeToChatRecordFile(withholdCurChatFile);
            } else {
                try {
                    // 生成当前应写入的完整内容
                    QString newContent;
                    for (int i = 0; i < messageWidgetList.size(); i++) {
                        MessageWidget *messageWidget = messageWidgetList.at(i);
                        if (!messageWidget)
                            continue;
                        newContent += messageWidget->getText() + '\n'
                                + QString("消息部件思考时长:%1秒\n")
                                          .arg(i < thinkTimeLengthList.size()
                                                       ? thinkTimeLengthList.at(i)
                                                       : 0)
                                + (messageWidget->getIsUser() ? "True\n" : "False\n");
                    }

                    // 读取现有文件内容（Text 模式读自动归一化换行）用于比较
                    QString oldContent;
                    QFile readFile(filePath);
                    if (readFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
                        QTextStream in(&readFile);
                        in.setEncoding(QStringConverter::Utf8);
                        oldContent = in.readAll();
                        readFile.close();
                    }

                    // 按完整内容比较而非行数：流式追加常为行内增长（换行数不变），
                    // 仅按行数判断会漏写文件，重建后消息将缺少最后追加的文本
                    if (oldContent != newContent) {
                        QFile writeFile(filePath);
                        if (writeFile.open(QIODevice::WriteOnly | QIODevice::Text
                                           | QIODevice::Truncate)) {
                            writeFile.resize(0);
                            writeFile.close();
                        }

                        QFile appendFile(filePath);
                        if (appendFile.open(QIODevice::WriteOnly | QIODevice::Text
                                            | QIODevice::Append)) {
                            QTextStream out(&appendFile);
                            out.setEncoding(QStringConverter::Utf8);
                            out << newContent;
                            appendFile.close();
                        }
                    }
                } catch (const std::exception &e) {
                    qDebug() << "发生未知错误：" << e.what();
                }
            }
        }
    }
}

void MainWindow::chatRecordsGenerateItem(QString searchText)
{
    QDir dir(appContext->chatRecordsDir());
    // 按文件名升序（文件名含时间戳，升序即时间序），addListItem 头部插入后最新记录在最上方
    QStringList fileNames = dir.entryList(QStringList() << "*.txt", QDir::Files, QDir::Name);
    for (const QString &fileName : fileNames) {
        QString filePath = dir.filePath(fileName);
        try {
            QStringList lines;
            QFile file(filePath);
            if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
                qDebug() << "错误：文件" << filePath << "不存在";
                continue;
            }
            QTextStream in(&file);
            in.setEncoding(QStringConverter::Utf8);
            QString content = in.readAll();
            file.close();

            // 有搜索词时先过滤，内容不含关键词的记录跳过
            if (!searchText.isEmpty() && !content.contains(searchText))
                continue;

            lines = content.split('\n');
            for (int i = 0; i < lines.size() - 1; i++)
                lines[i] += '\n';
            if (lines.last().isEmpty())
                lines.removeLast();

            // 定位首条用户消息结束标记（True\n），其后为 AI 回复首行；
            // 若为 <think>\n 则再跳一行，取思考或正文首行
            int index = 0;
            for (; index < lines.size(); index++) {
                if (lines[index] == "True\n") {
                    if (index + 1 < lines.size() && lines[index + 1] == "<think>\n")
                        index++;
                    break;
                }
            }
            if (lines.isEmpty() || index + 1 >= lines.size())
                continue;

            QString chatRecordStr = lines[0] + QString(lines[index + 1]).remove('\n');
            QListWidgetItem *item = chatRecordsWidget->addListItem(chatRecordStr);
            chatRecordsWidget->listItemSetData(item, fileName);
        } catch (const std::exception &e) {
            qDebug() << "发生未知错误：" << e.what();
        }
    }
}

void MainWindow::generateCurChatRecord(bool lastIsToggle, bool useThinkExpandList)
{
    QString text;
    bool isUser = true;
    thinkTimeLengthList.clear();
    int thinkTimeIndex = 0;
    int expandIndex = 0;

    QStringList lines;
    QString filePath = QDir(appContext->chatRecordsDir()).filePath(curChatFile);
    try {
        QFile file(filePath);
        if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            QTextStream in(&file);
            in.setEncoding(QStringConverter::Utf8);
            while (!in.atEnd()) {
                lines.append(in.readLine() + '\n');
            }
            file.close();
        }
    } catch (const std::exception &e) {
        qDebug() << "发生未知错误：" << e.what();
    }

    for (int i = 0; i < lines.size(); i++) {
        if (lines[i] == "True\n" || lines[i] == "False\n") {
            if (lines[i] == "True\n") {
                isUser = true;
            } else {
                isUser = false;
            }

            int j = messageWidgetList.size() - 1;
            if (j != -1 && j != 0) {
                if (!messageWidgetList[j]->getIsUser()) {
                    messageWidgetList[j]->removeRenewResponseButton();
                }
            }
            if (text.endsWith('\n'))
                text.chop(1);

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
                            messageWidgetTextMaxWidth());
                } else {
                    // 重建恢复历史消息，不传 executeNextFun，避免 TextShow 渲染完成后自动触发新线程
                    messageWidget = new MessageWidget(
                            appContext, text, [this]() { textCopy(); },
                            [this]() { messageRenewResponse(); },
                            [this](MessageWidget *selfMessageWidget) {
                                messageWidgetResize(selfMessageWidget);
                            },
                            [this](bool state) { getSetTexting(state); }, nullptr, chatShow,
                            thinkTimeLengthList, thinkTimeIndex, isUser, true,
                            messageWidgetTextMaxWidth());
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
                        thinkTimeLengthList, thinkTimeIndex, isUser, true,
                        messageWidgetTextMaxWidth());
            }
            // messageWidget->connectResizeFinished(this, &MainWindow::messageWidgetResize);
            // messageWidget->connectSetTexting(this, &MainWindow::getSetTexting);

            qDebug() << "generateCurChatRecord" << i;
            if (!isUser) {
                if (i == lines.size() - 1) {
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

            if (i == lines.size() - 1) {
                if (lastIsToggle) {
                    messageWidget->updateFunWidgetSize(curDpi, initDpi);
                    // messageWidget->toggleWidget();
                }
            } else {
                messageWidget->updateFunWidgetSize(curDpi, initDpi);
                // messageWidget->toggleWidget();
            }

            text.clear();

            thinkTimeIndex++;
            if (useThinkExpandList && !isUser) {
                expandIndex++;
            }

        } else if (lines[i].left(8) == QString("消息部件思考时长")) {
            QRegularExpression re("\\d+");
            QRegularExpressionMatch match = re.match(lines[i]);
            if (match.hasMatch()) {
                thinkTimeLengthList.append(match.captured(0).toInt());
            }
        } else {
            text += lines[i];
        }
    }
}

void MainWindow::getSetTexting(bool state)
{
    isSetTexting = state;
    // 渲染结束后若有待执行的重建，延迟到事件循环空闲时执行，避免在 setText 栈内重建
    if (!state && isRegeneratePending && !isRegenerating) {
        QTimer::singleShot(0, this, &MainWindow::messageWidgetRegenerate);
    }
}

void MainWindow::messageWidgetRegenerate()
{
    // 重入保护：正在重建或 AI 消息渲染中（嵌套事件循环内 WM_EXITSIZEMOVE 会被再次分发）
    // 再次触发时仅标记待重建，避免半成品控件重复创建与悬空指针访问
    if (isRegenerating || isSetTexting) {
        isRegeneratePending = true;
        return;
    }
    isRegenerating = true;
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

            // 重建期间置空接收指针，防止嵌套事件循环中消息回调访问旧控件
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
                messageRecvWidget = messageWidget;
                itemRecvHLayout = itemHLayout;
                itemRecvWidget = itemWidget;
                recvItem = item;
                isContinueShow = true;
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
                        HWND hwnd = reinterpret_cast<HWND>(winId());
                        DWORD style = GetWindowLong(hwnd, GWL_STYLE);
                        SetWindowLongPtr(hwnd, GWL_STYLE, style | WS_OVERLAPPEDWINDOW);
                        SetWindowPos(hwnd, NULL, 0, 0, 0, 0,
                                     SWP_FRAMECHANGED | SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER
                                             | SWP_NOOWNERZORDER);
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
                        delete messageWidgetList.takeLast();
                        int last = chatShow->count() - 1;
                        QWidget *itemWidget = chatShow->itemWidget(chatShow->item(last));
                        if (itemWidget)
                            itemWidget->deleteLater();
                        QListWidgetItem *lastItem = chatShow->takeItem(last);
                        delete lastItem;
                        messageRenewResponse();
                    }
                }
            }
        }
    } while (isRegeneratePending);
    isRegenerating = false;
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
    QDir dir(appContext->chatRecordsDir());
    const QStringList fileNames = dir.entryList(QStringList() << "*.txt", QDir::Files);
    for (const QString &fileName : fileNames) {
        if (!QFile::remove(dir.filePath(fileName)))
            qDebug() << "删除聊天记录文件失败：" << fileName;
    }
}

void MainWindow::generateChatRecord(QListWidgetItem *item)
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

    for (int i = 0; i < chatShow->count(); ++i) {
        QWidget *itemWidget = chatShow->itemWidget(chatShow->item(i));
        if (itemWidget)
            itemWidget->deleteLater();
    }
    chatShow->clear();
    // 切换聊天记录：恢复自动跟随，保证加载完成与后续追加都显示最新内容（底部）
    chatShow->resetScrollAutoChange();
    curChatFile = chatRecordsWidget->listItemToString(item);
    generateCurChatRecord();
}

void MainWindow::newChat()
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
    // 旧消息控件即将被销毁：停止接收链并断开裸指针（同 generateChatRecord）
    resetRecvChain();
    messageWidgetList.clear();
    thinkTimeLengthList.clear();
    for (int i = 0; i < chatShow->count(); i++) {
        QWidget *itemWidget = chatShow->itemWidget(chatShow->item(i));
        if (itemWidget)
            itemWidget->deleteLater();
    }
    chatShow->clear();
    // 新建聊天：恢复自动跟随，避免上一会话中滚动查看历史关闭的自动跟随
    // 残留（清空时 value 本就为 0，不触发 valueChanged 恢复）导致新会话不滚动
    chatShow->resetScrollAutoChange();
    curChatFile = "";
    pushButtonIsPress = true;
}
