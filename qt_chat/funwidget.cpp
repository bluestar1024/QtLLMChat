#include "funwidget.h"

FunWidget::FunWidget(QWidget *parent) : QWidget(parent)
{
    chatRecordsButton = new PushButton("聊天历史", 15, 35);
    chatRecordsButton->setFixedSize(44, 44);
    chatRecordsImagesPath = QDir(imagesDir).filePath("chat_records.png");
    chatRecordsButton->setIcon(QIcon(chatRecordsImagesPath));
    chatRecordsButton->setIconSize(QSize(30, 30));
    chatRecordsButton->setStyleSheet(
        "QPushButton{ border:none; border-radius:22px; }"
        "QPushButton:hover{ background:#d0d0d0; }"
    );
    funLeftSubWidget = new Widget();
    funLeftSubWidget->resize(chatRecordsButton->width() + 15,
                             chatRecordsButton->height() + 16);
    funLeftSubWidget->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);

    funLeftSubHLayout = new QHBoxLayout(funLeftSubWidget);
    funLeftSubHLayout->addWidget(chatRecordsButton);
    funLeftSubHLayout->setAlignment(Qt::AlignLeft);
    funLeftSubHLayout->setContentsMargins(10, 10, 5, 6);
    titleLabel = new QLabel();
    titleLabel->setFixedHeight(60);
    int fontId = QFontDatabase::addApplicationFont(fontFilePath);
    if (fontId != -1) {
        QString family = QFontDatabase::applicationFontFamilies(fontId).at(0);
        QFont font(family);
        font.setPixelSize(titleFontPixelSize);
        titleLabel->setFont(font);
    }
    titleLabel->setText("AI助理");
    titleLabel->adjustSize();
    titleLabel->setFixedWidth(titleLabel->width() + 2);
    funMidSubWidget = new Widget();
    funMidSubWidget->resize(titleLabel->size());
    funMidSubWidget->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);

    funMidSubHLayout = new QHBoxLayout(funMidSubWidget);
    funMidSubHLayout->addWidget(titleLabel);
    funMidSubHLayout->setAlignment(Qt::AlignCenter);
    funMidSubHLayout->setContentsMargins(0, 0, 0, 0);
    newChatButton = new PushButton("新聊天", 10, 35);
    newChatButton->setFixedSize(44, 44);
    newChatImagesPath = QDir(imagesDir).filePath("new_chat.png");
    newChatButton->setIcon(QIcon(newChatImagesPath));
    newChatButton->setIconSize(QSize(30, 30));
    newChatButton->setStyleSheet(
        "QPushButton{ border:none; border-radius:22px; }"
        "QPushButton:hover{ background:#d0d0d0; }"
    );
    funRightSubWidget = new Widget();
    funRightSubWidget->resize(newChatButton->width() + 15,
                              newChatButton->height() + 16);
    funRightSubWidget->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    funRightSubHLayout = new QHBoxLayout(funRightSubWidget);
    funRightSubHLayout->addWidget(newChatButton);
    funRightSubHLayout->setAlignment(Qt::AlignRight);
    funRightSubHLayout->setContentsMargins(5, 10, 10, 6);
    mainHLayout = new QHBoxLayout(this);
    mainHLayout->addWidget(funLeftSubWidget);
    mainHLayout->addWidget(funMidSubWidget);
    mainHLayout->addWidget(funRightSubWidget);
    mainHLayout->setContentsMargins(0, 0, 0, 0);

    setFixedSize(1200, 60);
    setMouseTracking(true);
    saveWidgetSize();
}

FunWidget::~FunWidget()
{
}

void FunWidget::mouseMoveEvent(QMouseEvent *event)
{
    QWidget::mouseMoveEvent(event);
    event->ignore();
}

void FunWidget::setSize()
{
    int side = (width() - funMidSubWidget->width()) / 2;
    funLeftSubWidget->setFixedSize(side, funLeftSubWidget->height());
    funRightSubWidget->setFixedSize(side, funRightSubWidget->height());
}

void FunWidget::saveWidgetSize()
{
    widgetSizeDict["this"] = size();
    widgetSizeDict["chatRecordsButton"] = chatRecordsButton->size();
    widgetSizeDict["chatRecordsButton iconSize"] = chatRecordsButton->iconSize();
    widgetSizeDict["funLeftSubWidget"] = funLeftSubWidget->size();
    widgetSizeDict["funLeftSubHLayout contentsMargins"] = QVariant::fromValue(funLeftSubHLayout->contentsMargins());
    widgetSizeDict["titleLabel"] = titleLabel->size();
    widgetSizeDict["funMidSubWidget"] = funMidSubWidget->size();
    widgetSizeDict["funMidSubHLayout contentsMargins"] = QVariant::fromValue(funMidSubHLayout->contentsMargins());
    widgetSizeDict["newChatButton"] = newChatButton->size();
    widgetSizeDict["newChatButton iconSize"] = newChatButton->iconSize();
    widgetSizeDict["funRightSubWidget"] = funRightSubWidget->size();
    widgetSizeDict["funRightSubHLayout contentsMargins"] = QVariant::fromValue(funRightSubHLayout->contentsMargins());
    widgetSizeDict["mainHLayout contentsMargins"] = QVariant::fromValue(mainHLayout->contentsMargins());
}

void FunWidget::resetWidgetSize()
{
    setSize();
    saveWidgetSize();
}

void FunWidget::updateSize(qreal curDpi, qreal lastDpi)
{
    if (lastDpi == 0) return;
    qreal ratio = qreal(curDpi / lastDpi);

    auto scale = [=](const QSize &s) -> QSize {
        return QSize(qRound(s.width() * ratio), qRound(s.height() * ratio));
    };
    auto scaleMargins = [=](const QMargins &m) -> QMargins {
        return QMargins(qRound(m.left() * ratio), qRound(m.top() * ratio),
                        qRound(m.right() * ratio), qRound(m.bottom() * ratio));
    };

    setFixedSize(scale(widgetSizeDict["this"].value<QSize>()));
    chatRecordsButton->setFixedSize(scale(widgetSizeDict["chatRecordsButton"].value<QSize>()));
    chatRecordsButton->setIconSize(scale(widgetSizeDict["chatRecordsButton iconSize"].value<QSize>()));
    chatRecordsButton->setStyleSheet(QString(
        "QPushButton{ border:none; border-radius:%1px; }"
        "QPushButton:hover{ background:#d0d0d0; }").arg(chatRecordsButton->width() / 2));

    funLeftSubWidget->setFixedSize(scale(widgetSizeDict["funLeftSubWidget"].value<QSize>()));
    funLeftSubHLayout->setContentsMargins(scaleMargins(widgetSizeDict["funLeftSubHLayout contentsMargins"].value<QMargins>()));

    titleLabel->setFixedSize(scale(widgetSizeDict["titleLabel"].value<QSize>()));
    funMidSubWidget->setFixedSize(scale(widgetSizeDict["funMidSubWidget"].value<QSize>()));
    funMidSubHLayout->setContentsMargins(0, 0, 0, 0);

    newChatButton->setFixedSize(scale(widgetSizeDict["newChatButton"].value<QSize>()));
    newChatButton->setIconSize(scale(widgetSizeDict["newChatButton iconSize"].value<QSize>()));
    newChatButton->setStyleSheet(QString(
        "QPushButton{ border:none; border-radius:%1px; }"
        "QPushButton:hover{ background:#d0d0d0; }").arg(newChatButton->width() / 2));

    funRightSubWidget->setFixedSize(scale(widgetSizeDict["funRightSubWidget"].value<QSize>()));
    funRightSubHLayout->setContentsMargins(scaleMargins(widgetSizeDict["funRightSubHLayout contentsMargins"].value<QMargins>()));
    mainHLayout->setContentsMargins(0, 0, 0, 0);
}
