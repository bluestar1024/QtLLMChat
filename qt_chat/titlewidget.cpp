#include "titlewidget.h"
#include "globalvariables.h"

TitleWidget::TitleWidget(QWidget *parent)
    : QWidget(parent),
      isRound(true),
      titleIconLabel(nullptr),
      titleLeftSubWidget(nullptr),
      titleRightSubWidget(nullptr),
      minButton(nullptr),
      maxButton(nullptr),
      closeButton(nullptr)
{
    setMouseTracking(true);
    initializeUI();
}

void TitleWidget::setRoundAngle()
{
    isRound = true;
    repaint();
}

void TitleWidget::setRightAngle()
{
    isRound = false;
    repaint();
}

TitleButton *TitleWidget::minimizeButton() const
{
    return minButton;
}

TitleButton *TitleWidget::maximizeButton() const
{
    return maxButton;
}

TitleButton *TitleWidget::closeButton() const
{
    return closeButton;
}

void TitleWidget::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    QPainterPath path;
    path.setFillRule(Qt::WindingFill);

    if (isRound) {
        path.addRoundedRect(rect().x(), rect().y(), rect().width(), rect().height(), 16, 16);
        path.addRect(rect().x(), rect().height() - 16, 16, 16);
        path.addRect(rect().width() - 16, rect().height() - 16, 16, 16);
    } else {
        path.addRect(rect().x(), rect().y(), rect().width(), rect().height());
    }

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

void TitleWidget::initializeUI()
{
    setupLeftWidget();
    setupRightWidget();
    setupMainLayout();
}

void TitleWidget::setupLeftWidget()
{
    titleIconLabel = new QLabel(this);
    titleIconLabel->setFixedSize(40, 30);
    titleIconLabel->setScaledContents(true);

    QString aiAssistantPath = imagesDir + "/ai_assistant.png";
    titleIconLabel->setPixmap(QPixmap(aiAssistantPath));

    titleLeftSubWidget = new QWidget(this);
    titleLeftSubWidget->resize(titleIconLabel->width() + 15, titleIconLabel->height() + 10);
    titleLeftSubWidget->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);

    QHBoxLayout *leftLayout = new QHBoxLayout(titleLeftSubWidget);
    leftLayout->addWidget(titleIconLabel);
    leftLayout->setAlignment(Qt::AlignLeft);
    leftLayout->setContentsMargins(10, 5, 5, 5);
}

void TitleWidget::setupRightWidget()
{
    minButton = new TitleButton("", 10, 35, this);
    minButton->setFixedSize(50, 40);
    QString minPath = imagesDir + "/min.png";
    minButton->setIcon(QIcon(minPath));
    minButton->setIconSize(QSize(20, 20));
    minButton->setStyleSheet("QPushButton { border: none; }"
                             "QPushButton:hover { background: #808080; }");
    connect(minButton, &TitleButton::clicked, this, &TitleWidget::minimizeClicked);

    maxButton = new TitleButton("", 10, 35, this);
    maxButton->setFixedSize(50, 40);
    maxPath = imagesDir + "/max.png";
    normalPath = imagesDir + "/normal.png";
    maxButton->setIcon(QIcon(maxPath));
    maxButton->setIconSize(QSize(20, 20));
    maxButton->setStyleSheet("QPushButton { border: none; }"
                             "QPushButton:hover { background: #808080; }");
    connect(maxButton, &TitleButton::clicked, this, &TitleWidget::maximizeClicked);

    closeButton = new TitleButton("", 5, 35, this);
    closeButton->setFixedSize(50, 40);
    QString closePath = imagesDir + "/close.png";
    closeButton->setIcon(QIcon(closePath));
    closeButton->setIconSize(QSize(20, 20));
    closeButton->setStyleSheet("QPushButton { border: none; }"
                               "QPushButton:hover {"
                               "   border-top-right-radius: 16px;"
                               "   background: #c80000;"
                               "}");
    connect(closeButton, &TitleButton::clicked, this, &TitleWidget::closeClicked);

    titleRightSubWidget = new QWidget(this);
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
    QHBoxLayout *mainLayout = new QHBoxLayout(this);
    mainLayout->addWidget(titleLeftSubWidget);
    mainLayout->addWidget(titleRightSubWidget);
    mainLayout->setContentsMargins(0, 0, 0, 0);

    setLayout(mainLayout);
    resize(1200, 40);
    setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
}
