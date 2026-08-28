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
            QTimer::singleShot(10, this, [this]() { applyDWMShadow(); });
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
        if (msg->wParam == SIZE_RESTORED || msg->wParam == SIZE_MAXIMIZED) {
            QTimer::singleShot(10, this, &MainWindow::applyDWMShadow);
            // 双击标题栏/单击最大化按钮等非拖拽方式的最大化/还原：
            // 不经过 WM_ENTERSIZEMOVE/WM_EXITSIZEMOVE，只能在这里触发重建；
            // 与上次重建尺寸相同（如 Snap 后延迟到达的 WM_SIZE）则跳过
            if (!isShowFirst && !isSizeMoveDrag) {
                RECT rect;
                GetWindowRect(hwnd, &rect);
                int w = rect.right - rect.left;
                int h = rect.bottom - rect.top;
                if (w != lastRegenerateWidth || h != lastRegenerateHeight) {
                    lastRegenerateWidth = w;
                    lastRegenerateHeight = h;
                    qDebug() << "WM_SIZE isRegenerate" << w << h;
                    // 延迟到 Qt resizeEvent 执行后再重建，保证 chatShow 宽度已更新
                    QTimer::singleShot(0, this, &MainWindow::messageWidgetRegenerate);
                } else {
                    qDebug() << "WM_SIZE skip regenerate same size" << w << h;
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
        // 目标尺寸发生变化（最大化/半屏还原、Aero Snap）时仅标记待重建，
        // 不在这里直接重建：系统拖拽模态循环中执行耗时重建会阻塞拖拽造成卡顿，
        // 且 Snap 时 Qt resizeEvent 尚未执行（chatShow 宽度未同步），重建会读旧宽度
        if (dragRegenerateDone)
            break;
        RECT *rc = reinterpret_cast<RECT *>(msg->lParam);
        int w = rc->right - rc->left;
        int h = rc->bottom - rc->top;
        if (w != dragStartWidth || h != dragStartHeight) {
            dragRegenerateDone = true;
            pendingRegenerateAfterResize = true;
            qDebug() << "WM_MOVING size changed, pending regenerate" << w << h;
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
            if (w != dragStartWidth || h != dragStartHeight) {
                // 拖拽结束、事件循环恢复后执行：此时 Qt resizeEvent 已同步新宽度，
                // 重建宽度正确且不阻塞拖拽过程（重建不在模态循环中执行）
                lastRegenerateWidth = w;
                lastRegenerateHeight = h;
                qDebug() << "WM_EXITSIZEMOVE regenerate pending" << w << h;
                QTimer::singleShot(0, this, &MainWindow::messageWidgetRegenerate);
            } else {
                qDebug() << "WM_EXITSIZEMOVE pending canceled, size back to start";
            }
        } else if (dragRegenerateDone) {
            // 拖动过程中已重建过；若最终尺寸与重建时不同（继续拉伸）则补一次
            if (w != lastRegenerateWidth || h != lastRegenerateHeight) {
                lastRegenerateWidth = w;
                lastRegenerateHeight = h;
                qDebug() << "WM_EXITSIZEMOVE size still changed, regenerate" << w << h;
                QTimer::singleShot(0, this, &MainWindow::messageWidgetRegenerate);
            } else {
                qDebug() << "WM_EXITSIZEMOVE already regenerated, skip";
            }
        } else if (w != dragStartWidth || h != dragStartHeight) {
            // 拖动中未触发（如 Snap 尺寸变化发生在最后时刻）：现在重建
            lastRegenerateWidth = w;
            lastRegenerateHeight = h;
            qDebug() << "WM_EXITSIZEMOVE size changed, regenerate" << w << h;
            QTimer::singleShot(0, this, &MainWindow::messageWidgetRegenerate);
        } else {
            qDebug() << "WM_EXITSIZEMOVE move only, skip regenerate";
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
        chatFun->setFixedSize(mainWidget->width() * 2 / 3, chatFun->height());
        chatFun->resetWidgetSize();
        chatShow->resize(mainWidget->width() * 2 / 3 - 29, chatShow->height());
        chatShowWidget->resize(mainWidget->width() * 2 / 3, chatShowWidget->height());
        chatInput->resize(mainWidget->width() * 2 / 3 - 40, chatInput->height());
        chatInput->resetWidgetSize();
        chatInputWidget->resize(mainWidget->width() * 2 / 3, chatInputWidget->height());
        splitter->resize(mainWidget->width() * 2 / 3, splitter->height());
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
    chatShow->resize(mainWidget->width() - rect.x() - chatRecordsWidget->width() - 29,
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
}

void MainWindow::chatRecordsUiMoveFinished()
{
    chatFun->saveWidgetSize();
    if (!chatRecordsWidgetIsOpen)
        chatRecordsWidget->delAllListItems();
    messageWidgetRegenerate();
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
        MessageWidget *messageWidget = messageWidgetList.at(i);
        // messageWidget->setSize();
        QListWidgetItem *item = chatShow->item(i);
        QWidget *itemWidget = chatShow->itemWidget(item);
        if (!itemWidget)
            return;
        const int w = chatShow->width();
        const int h = messageWidget->height() + 10;
        itemWidget->setFixedSize(w, h);
        QLayout *itemLayout = itemWidget->layout();
        if (itemLayout) {
            if (messageWidget->getIsUser())
                itemLayout->setContentsMargins(itemWidget->width() - messageWidget->width() - 25, 5,
                                               25, 5);
            else
                itemLayout->setContentsMargins(0, 5, itemWidget->width() - messageWidget->width(),
                                               5);
        }
        item->setSizeHint(QSize(w, h));
    }
    qDebug() << "messageWidgetResize end";
}

void MainWindow::sendMessage()
{
    if (!isSending) {
        QList<QVariantMap> context = {};
        QString text = chatInput->toPlainText().trimmed();
        if (!text.isEmpty()) {
            for (auto *w : messageWidgetList) {
                QVariantMap m;
                m["role"] = w->getIsUser() ? "user" : "assistant";
                m["content"] = w->getText();
                context.append(m);
            }
            thinkTimeLengthList.append(0);
            messageSendWidget = new MessageWidget(
                    appContext, text, [this]() { textCopy(); }, [this]() { messageRenewResponse(); },
                    [this](MessageWidget *selfMessageWidget) {
                        messageWidgetResize(selfMessageWidget);
                    },
                    [this](bool state) { getSetTexting(state); }, [this]() { onExecuteNext(); },
                    chatShow, thinkTimeLengthList, messageWidgetList.size(), true, true,
                    chatShow->width() * 3 / 4);
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
            itemSendHLayout->setContentsMargins(
                    itemSendWidget->width() - messageSendWidget->width() - 25, 5, 25, 5);

            sendItem = new QListWidgetItem(chatShow);
            sendItem->setSizeHint(QSize(chatShow->width(), messageSendWidget->height() + 10));
            chatShow->setItemWidget(sendItem, itemSendWidget);

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
            messageWidgetList.size(), false, true, chatShow->width() * 3 / 4);
    // messageRecvWidget->connectResizeFinished(this, &MainWindow::messageWidgetResize);
    // messageRecvWidget->connectSetTexting(this, &MainWindow::getSetTexting);
    messageWidgetList.append(messageRecvWidget);

    itemRecvWidget = new ItemWidget(this);
    itemRecvHLayout = new QHBoxLayout(itemRecvWidget);
    itemRecvHLayout->addWidget(messageRecvWidget);
    itemRecvWidget->setFixedSize(chatShow->width(), messageRecvWidget->height() + 10);
    itemRecvHLayout->setContentsMargins(0, 5, itemRecvWidget->width() - messageRecvWidget->width(),
                                        5);

    recvItem = new QListWidgetItem(chatShow);
    recvItem->setSizeHint(QSize(chatShow->width(), messageRecvWidget->height() + 10));
    chatShow->setItemWidget(recvItem, itemRecvWidget);

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
        itemRecvWidget->setFixedSize(chatShow->width(), messageRecvWidget->height() + 10);
        itemRecvHLayout->setContentsMargins(
                0, 5, itemRecvWidget->width() - messageRecvWidget->width(), 5);
        recvItem->setSizeHint(QSize(chatShow->width(), messageRecvWidget->height() + 10));
    }

    HWND hwnd = reinterpret_cast<HWND>(winId());
    DWORD style = GetWindowLong(hwnd, GWL_STYLE);
    SetWindowLongPtr(hwnd, GWL_STYLE, style | WS_OVERLAPPEDWINDOW);
    SetWindowPos(hwnd, NULL, 0, 0, 0, 0,
                 SWP_FRAMECHANGED | SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOOWNERZORDER);

    qDebug() << "recvMessage: setText finish";
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
        if (itemRecvWidget && itemRecvHLayout && recvItem) {
            itemRecvWidget->setFixedSize(chatShow->width(), messageRecvWidget->height() + 10);
            itemRecvHLayout->setContentsMargins(
                    0, 5, itemRecvWidget->width() - messageRecvWidget->width(), 5);
            recvItem->setSizeHint(QSize(chatShow->width(), messageRecvWidget->height() + 10));
        }
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
        itemRecvWidget->setFixedSize(chatShow->width(), messageRecvWidget->height() + 10);
        itemRecvHLayout->setContentsMargins(
                0, 5, itemRecvWidget->width() - messageRecvWidget->width(), 5);
        recvItem->setSizeHint(QSize(chatShow->width(), messageRecvWidget->height() + 10));
    }

    if (message.isEmpty() && !messageWidgetList.isEmpty() && chatShow->count() > 0) {
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
    Q_UNUSED(searchText);
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
                            chatShow->width() * 3 / 4);
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
                            chatShow->width() * 3 / 4);
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
                        chatShow->width() * 3 / 4);
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
            if (isUser) {
                itemHLayout->setContentsMargins(itemWidget->width() - messageWidget->width() - 25,
                                                5, 25, 5);
            } else {
                itemHLayout->setContentsMargins(0, 5, itemWidget->width() - messageWidget->width(),
                                                5);
            }
            item = new QListWidgetItem(chatShow);
            item->setSizeHint(QSize(chatShow->width(), messageWidget->height() + 10));
            chatShow->setItemWidget(item, itemWidget);

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
                        if (itemRecvWidget && itemRecvHLayout && recvItem) {
                            itemRecvWidget->setFixedSize(chatShow->width(),
                                                         messageRecvWidget->height() + 10);
                            itemRecvHLayout->setContentsMargins(
                                    0, 5, itemRecvWidget->width() - messageRecvWidget->width(), 5);
                            recvItem->setSizeHint(
                                    QSize(chatShow->width(), messageRecvWidget->height() + 10));
                        }
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
                        itemRecvWidget->setFixedSize(chatShow->width(),
                                                     messageRecvWidget->height() + 10);
                        itemRecvHLayout->setContentsMargins(
                                0, 5, itemRecvWidget->width() - messageRecvWidget->width(), 5);
                        recvItem->setSizeHint(
                                QSize(chatShow->width(), messageRecvWidget->height() + 10));
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

void MainWindow::showChatRecords() { }

void MainWindow::showSearchRecords()
{
    QString text = chatRecordsWidget->getLineEditText();
    chatRecordsWidget->delAllListItems();
    chatRecordsGenerateItem(text);
}

void MainWindow::clearAllChatRecords() { }

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
    messageWidgetList.clear();

    for (int i = 0; i < chatShow->count(); ++i) {
        QWidget *itemWidget = chatShow->itemWidget(chatShow->item(i));
        if (itemWidget)
            itemWidget->deleteLater();
    }
    chatShow->clear();
    curChatFile = chatRecordsWidget->listItemToString(item);
    generateCurChatRecord();
}

void MainWindow::newChat() { }
