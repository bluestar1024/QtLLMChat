#include "titlewidget.h"
#include "appcontext.h"

TitleWidget::TitleWidget(AppContext *appContext, QWidget *parent)
    : QWidget(parent),
      appContext(appContext),
      // isRound(true),
      titleIconLabel(nullptr),
      minButton(nullptr),
      maxButton(nullptr),
      closeButton(nullptr),
      titleLeftSubWidget(nullptr),
      titleRightSubWidget(nullptr)
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
    minButton = new TitleButton(appContext, "最小化", 10, 35);
    minButton->setFixedSize(50, 40);
    QString minImagePath = appContext->imagesDir() + "/min.png";
    minButton->setIcon(QIcon(minImagePath));
    minButton->setIconSize(QSize(20, 20));
    minButton->setStyleSheet("QPushButton { border: none; }"
                             "QPushButton:hover { background: #808080; }");
    connect(minButton, &QPushButton::clicked, this, &TitleWidget::minimizeClicked);

    maxButton = new TitleButton(appContext, "最大化", 10, 35);
    maxButton->setFixedSize(50, 40);
    maxImagePath = appContext->imagesDir() + "/max.png";
    normalImagePath = appContext->imagesDir() + "/normal.png";
    maxButton->setIcon(QIcon(maxImagePath));
    maxButton->setIconSize(QSize(20, 20));
    maxButton->setStyleSheet("QPushButton { border: none; }"
                             "QPushButton:hover { background: #808080; }");
    connect(maxButton, &QPushButton::clicked, this, &TitleWidget::maximizeClicked);

    closeButton = new TitleButton(appContext, "关闭", 5, 35);
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
        maxButton->setTipOffset(10, 35);
    } else {
        maxButton->setIcon(QIcon(normalImagePath));
        maxButton->setTipText("正常");
        maxButton->setTipOffset(5, 35);
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
    widgetSizeDict["titleIconLabel"] = titleIconLabel->size();
    widgetSizeDict["titleLeftSubWidget"] = titleLeftSubWidget->size();
    widgetSizeDict["minButton"] = minButton->size();
    widgetSizeDict["minButton iconSize"] = minButton->iconSize();
    widgetSizeDict["maxButton"] = maxButton->size();
    widgetSizeDict["maxButton iconSize"] = maxButton->iconSize();
    widgetSizeDict["closeButton"] = closeButton->size();
    widgetSizeDict["closeButton iconSize"] = closeButton->iconSize();
    widgetSizeDict["titleRightSubWidget"] = titleRightSubWidget->size();
    widgetSizeDict["titleWidget"] = size();
}

void TitleWidget::updateSize(qreal curDpi, qreal lastDpi)
{
    if (lastDpi == 0)
        return;
    qreal ratio = qreal(curDpi / lastDpi);

    auto scale = [=](const QSize &s) -> QSize {
        return QSize(qRound(s.width() * ratio), qRound(s.height() * ratio));
    };

    titleIconLabel->setFixedSize(scale(widgetSizeDict["titleIconLabel"].value<QSize>()));
    titleLeftSubWidget->resize(scale(widgetSizeDict["titleLeftSubWidget"].value<QSize>()));
    minButton->setFixedSize(scale(widgetSizeDict["minButton"].value<QSize>()));
    minButton->setIconSize(scale(widgetSizeDict["minButton iconSize"].value<QSize>()));
    maxButton->setFixedSize(scale(widgetSizeDict["maxButton"].value<QSize>()));
    maxButton->setIconSize(scale(widgetSizeDict["maxButton iconSize"].value<QSize>()));
    closeButton->setFixedSize(scale(widgetSizeDict["closeButton"].value<QSize>()));
    closeButton->setIconSize(scale(widgetSizeDict["closeButton iconSize"].value<QSize>()));
    titleRightSubWidget->resize(scale(widgetSizeDict["titleRightSubWidget"].value<QSize>()));
    setFixedHeight(qRound(widgetSizeDict["self"].value<QSize>().height() * ratio));
}
