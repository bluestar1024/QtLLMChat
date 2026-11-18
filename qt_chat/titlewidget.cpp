#include "titlewidget.h"
#include "appcontext.h"
#include "dpiutil.h"

TitleWidget::TitleWidget(AppContext *appContext, QWidget *parent)
    : QWidget(parent),
      appContext(appContext),
      // isRound(true),
      titleIconLabel(nullptr),
      minButton(nullptr),
      maxButton(nullptr),
      closeButton(nullptr),
      titleLeftSubWidget(nullptr),
      titleRightSubWidget(nullptr),
      // 构造期的尺寸是代码默认值，即 144 DPI 基准尺寸
      appliedDpi(DpiUtil::kBaseDpi)
{
    setMouseTracking(true);
    initializeUI();
}

TitleWidget::~TitleWidget() { }

// void TitleWidget::setRoundAngle()
// {
//     isRound = true;
//     repaint();
// }

// void TitleWidget::setRightAngle()
// {
//     isRound = false;
//     repaint();
// }

void TitleWidget::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    QPainterPath path;
    path.setFillRule(Qt::WindingFill);

    // if (isRound) {
    //     path.addRoundedRect(rect().x(), rect().y(), rect().width(), rect().height(), 16, 16);
    //     path.addRect(rect().x(), rect().height() - 16, 16, 16);
    //     path.addRect(rect().width() - 16, rect().height() - 16, 16, 16);
    // } else {
    //     path.addRect(rect().x(), rect().y(), rect().width(), rect().height());
    // }
    path.addRect(rect().x(), rect().y(), rect().width(), rect().height());

    QBrush brush(Qt::SolidPattern);
    brush.setColor(QColor(60, 60, 60));

    painter.setPen(Qt::NoPen);
    painter.setBrush(brush);
    painter.drawPath(path.simplified());
}

void TitleWidget::mouseMoveEvent(QMouseEvent *event)
{
    QWidget::mouseMoveEvent(event);
    event->ignore();
}

// void TitleWidget::mousePressEvent(QMouseEvent *e)
// {
//     QWidget::mousePressEvent(e);
//     e->ignore();
// }

// void TitleWidget::mouseReleaseEvent(QMouseEvent *e)
// {
//     QWidget::mouseReleaseEvent(e);
//     e->ignore();
// }

void TitleWidget::initializeUI()
{
    setupLeftWidget();
    setupRightWidget();
    setupMainLayout();
}

void TitleWidget::setupLeftWidget()
{
    titleIconLabel = new QLabel();
    titleIconLabel->setFixedSize(40, 30);
    titleIconLabel->setScaledContents(true);

    QString aiAssistantImagePath = appContext->imagesDir() + "/ai_assistant.png";
    titleIconLabel->setPixmap(QPixmap(aiAssistantImagePath));

    titleLeftSubWidget = new Widget();
    titleLeftSubWidget->resize(titleIconLabel->width() + 15, titleIconLabel->height() + 10);
    titleLeftSubWidget->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);

    QHBoxLayout *leftLayout = new QHBoxLayout(titleLeftSubWidget);
    leftLayout->addWidget(titleIconLabel);
    leftLayout->setAlignment(Qt::AlignLeft);
    leftLayout->setContentsMargins(10, 5, 5, 5);
}

void TitleWidget::setupRightWidget()
{
    minButton = new TitleButton(appContext, "最小化");
    minButton->setFixedSize(50, 40);
    QString minImagePath = appContext->imagesDir() + "/min.png";
    minButton->setIcon(QIcon(minImagePath));
    minButton->setIconSize(QSize(20, 20));
    minButton->setStyleSheet("QPushButton { border: none; }"
                             "QPushButton:hover { background: #808080; }");
    connect(minButton, &QPushButton::clicked, this, &TitleWidget::minimizeClicked);

    maxButton = new TitleButton(appContext, "最大化");
    maxButton->setFixedSize(50, 40);
    maxImagePath = appContext->imagesDir() + "/max.png";
    normalImagePath = appContext->imagesDir() + "/normal.png";
    maxButton->setIcon(QIcon(maxImagePath));
    maxButton->setIconSize(QSize(20, 20));
    maxButton->setStyleSheet("QPushButton { border: none; }"
                             "QPushButton:hover { background: #808080; }");
    connect(maxButton, &QPushButton::clicked, this, &TitleWidget::maximizeClicked);

    closeButton = new TitleButton(appContext, "关闭");
    closeButton->setFixedSize(50, 40);
    QString closeImagePath = appContext->imagesDir() + "/close.png";
    closeButton->setIcon(QIcon(closeImagePath));
    closeButton->setIconSize(QSize(20, 20));
    closeButton->setStyleSheet("QPushButton { border: none; }"
                               "QPushButton:hover { background: #c80000; }");
    connect(closeButton, &QPushButton::clicked, this, &TitleWidget::closeClicked);

    titleRightSubWidget = new Widget();
    titleRightSubWidget->resize(150, 40);
    titleRightSubWidget->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);

    QHBoxLayout *rightLayout = new QHBoxLayout(titleRightSubWidget);
    rightLayout->addWidget(minButton);
    rightLayout->addWidget(maxButton);
    rightLayout->addWidget(closeButton);
    rightLayout->setAlignment(Qt::AlignRight);
    rightLayout->setContentsMargins(0, 0, 0, 0);
    rightLayout->setSpacing(0);
}

void TitleWidget::setupMainLayout()
{
    resize(1200, 40);
    setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);

    QHBoxLayout *mainLayout = new QHBoxLayout(this);
    mainLayout->addWidget(titleLeftSubWidget);
    mainLayout->addWidget(titleRightSubWidget);
    mainLayout->setContentsMargins(0, 0, 0, 0);
}

void TitleWidget::maxButtonToggleIcon(bool isMaxIcon)
{
    if (isMaxIcon) {
        maxButton->setIcon(QIcon(maxImagePath));
        maxButton->setTipText("最大化");
    } else {
        maxButton->setIcon(QIcon(normalImagePath));
        maxButton->setTipText("正常");
    }
}

QSize TitleWidget::getMinButtonSize()
{
    return minButton->size();
}

QSize TitleWidget::getMaxButtonSize()
{
    return maxButton->size();
}

QSize TitleWidget::getCloseButtonSize()
{
    return closeButton->size();
}

void TitleWidget::saveWidgetSize()
{
    // 全部按 144 基准保存（appliedDpi 为这些尺寸实际所属的 DPI）
    widgetSizeDict["titleIconLabel"] = DpiUtil::toBaseSize(titleIconLabel->size(), appliedDpi);
    widgetSizeDict["titleLeftSubWidget"] =
            DpiUtil::toBaseSize(titleLeftSubWidget->size(), appliedDpi);
    widgetSizeDict["minButton"] = DpiUtil::toBaseSize(minButton->size(), appliedDpi);
    widgetSizeDict["minButton iconSize"] = DpiUtil::toBaseSize(minButton->iconSize(), appliedDpi);
    widgetSizeDict["maxButton"] = DpiUtil::toBaseSize(maxButton->size(), appliedDpi);
    widgetSizeDict["maxButton iconSize"] = DpiUtil::toBaseSize(maxButton->iconSize(), appliedDpi);
    widgetSizeDict["closeButton"] = DpiUtil::toBaseSize(closeButton->size(), appliedDpi);
    widgetSizeDict["closeButton iconSize"] =
            DpiUtil::toBaseSize(closeButton->iconSize(), appliedDpi);
    widgetSizeDict["titleRightSubWidget"] =
            DpiUtil::toBaseSize(titleRightSubWidget->size(), appliedDpi);
    widgetSizeDict["titleWidget"] = DpiUtil::toBaseSize(size(), appliedDpi);
}

void TitleWidget::updateSize(qreal curDpi, qreal lastDpi)
{
    // 尺寸以 144 DPI 基准（DpiUtil::kBaseDpi）绝对换算，与上一次 DPI（lastDpi）
    // 无关：缓存里存的就是基准值，反复跨屏不会因逐次取整而漂移
    Q_UNUSED(lastDpi);
    if (curDpi <= 0)
        curDpi = DpiUtil::kBaseDpi;
    // resizeEvent 中 updateSize 先于 saveWidgetSize 执行：首次 DPI 缩放若发生在
    // 首个 resizeEvent 之前，widgetSizeDict 为空，各键取默认 QSize(-1,-1) 缩放
    // 出的负尺寸会把按钮/图标挤没、标题栏高度失效（界面失去 TitleWidget）。
    // 此时先以当前尺寸为基准保存再缩放：与正常路径（上轮 resizeEvent 已保存
    // 旧 DPI 尺寸）结果一致
    if (widgetSizeDict.isEmpty()) {
        saveWidgetSize();
        qDebug() << "TitleWidget::updateSize dict empty, saved current as baseline";
    }

    titleIconLabel->setFixedSize(DpiUtil::scaleSize(widgetSizeDict["titleIconLabel"].value<QSize>(),
                                                    curDpi));
    titleLeftSubWidget->resize(
            DpiUtil::scaleSize(widgetSizeDict["titleLeftSubWidget"].value<QSize>(), curDpi));
    minButton->setFixedSize(DpiUtil::scaleSize(widgetSizeDict["minButton"].value<QSize>(), curDpi));
    minButton->setIconSize(
            DpiUtil::scaleSize(widgetSizeDict["minButton iconSize"].value<QSize>(), curDpi));
    maxButton->setFixedSize(DpiUtil::scaleSize(widgetSizeDict["maxButton"].value<QSize>(), curDpi));
    maxButton->setIconSize(
            DpiUtil::scaleSize(widgetSizeDict["maxButton iconSize"].value<QSize>(), curDpi));
    closeButton->setFixedSize(
            DpiUtil::scaleSize(widgetSizeDict["closeButton"].value<QSize>(), curDpi));
    closeButton->setIconSize(
            DpiUtil::scaleSize(widgetSizeDict["closeButton iconSize"].value<QSize>(), curDpi));
    titleRightSubWidget->resize(
            DpiUtil::scaleSize(widgetSizeDict["titleRightSubWidget"].value<QSize>(), curDpi));
    // 键名与 saveWidgetSize 保持一致（"titleWidget"）
    setFixedHeight(DpiUtil::scalePx(widgetSizeDict["titleWidget"].value<QSize>().height(), curDpi));
    appliedDpi = curDpi;
    qDebug() << "TitleWidget::updateSize dpi" << curDpi << "newHeight" << height();
}
