#include "messagewidget.h"

#include <QTimer>

MessageWidget::MessageWidget(const QString &text, std::function<void()> copyFun,
                             std::function<void()> renewResponseFun, ListWidget *listWidget,
                             QList<int> &thinkTimeLengthList, int thinkTimeIndex, bool isUser,
                             bool thinkIsExpand, int textMaxWidth, QWidget *parent)
    : QWidget(parent),
      text(text),
      copyFun(copyFun),
      renewResponseFun(renewResponseFun),
      listWidget(listWidget),
      thinkTimeLengthList(thinkTimeLengthList),
      thinkTimeIndex(thinkTimeIndex),
      isUser(isUser),
      thinkIsExpand(thinkIsExpand),
      textMaxWidth(textMaxWidth - 10)
{
    layout = new QHBoxLayout(this);
    layout->setContentsMargins(5, 5, 5, 5);
    textShow = new TextShow(this->text, this->textMaxWidth, this);
    layout->addWidget(textShow);
    // if (!isUser) layout->addStretch();
    // else           layout->setAlignment(Qt::AlignRight);
    setFixedSize(textShow->size() + QSize(10, 0));
    setObjectName("messageWidget");
    setStyleSheet("#messageWidget{"
                  "background: green;"
                  "}");
}

MessageWidget::MessageWidget(const QString &text, std::function<void()> copyFun,
                             std::function<void()> renewResponseFun, bool isUser, int textMaxWidth,
                             QWidget *parent)
    : QWidget(parent),
      text(text),
      copyFun(std::move(copyFun)),
      renewResponseFun(std::move(renewResponseFun)),
      isUser(isUser),
      textMaxWidth(textMaxWidth)
{
    setMouseTracking(true);
    aiUpdateSizeTimer.setSingleShot(true);
    connect(&aiUpdateSizeTimer, &QTimer::timeout, this, &MessageWidget::onAiUpdateSize);

    imageLabel = new ImageLabel(isUser, this);

    textWidget = new QWidget(this);
    textLayout = new QVBoxLayout(textWidget);
    textLayout->setContentsMargins(15, 5, 15, 5);

    textBoxWidget = new QWidget(this);
    textBoxLayout = new QVBoxLayout(textBoxWidget);
    textBoxLayout->setSpacing(0);
    textBoxLayout->setContentsMargins(0, 0, 0, 0);

    funWidget = new QWidget(this);
    auto *funLay = new QHBoxLayout(funWidget);
    funLay->setContentsMargins(5, 5, 5, 5);
    copyButton = new QPushButton(funWidget);
    copyButton->setFixedSize(16, 16);
    copyButton->setCursor(Qt::PointingHandCursor);
    copyButton->setToolTip("复制");
    copyButton->setStyleSheet("QPushButton{ border-image:url(:/icons/copy.png); }"
                              "QPushButton:hover{ border-image:url(:/icons/copy_hover.png); }");
    connect(copyButton, &QPushButton::clicked, copyFun);
    funLay->addWidget(copyButton);
    copyButton->hide();

    if (!isUser) {
        renewButton = new QPushButton(funWidget);
        renewButton->setFixedSize(16, 16);
        renewButton->setToolTip("重新生成响应");
        renewButton->setStyleSheet(
                "QPushButton{ border-image:url(:/icons/renewResponse.png); }"
                "QPushButton:hover{ border-image:url(:/icons/renewResponse_hover.png); }");
        connect(renewButton, &QPushButton::clicked, renewFun);
        funLay->addWidget(renewButton);
        renewButton->hide();
        renewButtonIsRemove = false;
        funWidget->setFixedSize(52, 26);
    } else {
        funWidget->setFixedSize(26, 26);
    }

    if (isUser)
        buildUserUi();
    else
        buildAiUi();

    auto *mainLay = new QHBoxLayout(this);
    mainLay->setContentsMargins(0, 0, 0, 0);
    mainLay->setSpacing(5);
    if (isUser) {
        mainLay->addWidget(textBoxWidget);
        mainLay->addWidget(imageLabel);
    } else {
        mainLay->addWidget(imageLabel);
        mainLay->addWidget(textBoxWidget);
    }
    setFixedSize(imageLabel->width() + textBoxWidget->width() + 5,
                 qMax(imageLabel->height(), textBoxWidget->height()));
}

MessageWidget::~MessageWidget() { }

bool MessageWidget::getIsUser()
{
    return isUser;
}

QString MessageWidget::getText()
{
    return text;
}

void MessageWidget::updateFunWidgetSize(qreal curDpi, qreal initDpi)
{
    Q_UNUSED(curDpi);
    Q_UNUSED(initDpi);
}

void MessageWidget::toggleWidget()
{
    connect(textShow, &TextShow::setSizeFinished, this, &MessageWidget::onSizeFinshed);
    textShow->toggleWidget();
}

void MessageWidget::breakHandle() { }

void MessageWidget::removeRenewResponseButton() { }

void MessageWidget::setSize()
{
    setFixedSize(textShow->size() + QSize(10, 0));
}

void MessageWidget::setText(const QString &text)
{
    this->text = text;
    textShow->setText(text);
    setFixedSize(textShow->size() + QSize(10, 0));
}

void MessageWidget::removeLoadingWidget() { }

void MessageWidget::onSizeFinshed()
{
    emit resizeFinished();
    if (!isUser)
        emit setTexting(false);
}

ListWidget *MessageWidget::getListWidget()
{
    return listWidget;
}

bool MessageWidget::hasSelectedText()
{
    return false;
}

QString MessageWidget::getSelectedText()
{
    return "";
}
