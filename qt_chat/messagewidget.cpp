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
    textShow = new TextShow(this->text, this->isUser, this->textMaxWidth, this);
    layout->addWidget(textShow);
    // if (!isUser) layout->addStretch();
    // else           layout->setAlignment(Qt::AlignRight);
    setFixedSize(textShow->size() + QSize(10, 0));
    setObjectName("messageWidget");
    setStyleSheet("#messageWidget{"
                  "background: green;"
                  "}");
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
