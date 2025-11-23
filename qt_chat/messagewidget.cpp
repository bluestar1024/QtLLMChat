#include "messagewidget.h"
#include <QTimer>

MessageWidget::MessageWidget(const QString &text,
                             std::function<void()> copyFun,
                             std::function<void()> renewResponseFun,
                             QListWidget *listWidget,
                             QList<int> &thinkTimeLengthList,
                             int thinkTimeIndex,
                             bool isUser,
                             bool thinkIsExpand,
                             int textMaxWidth,
                             QWidget *parent)
    : QWidget(parent),
      m_text(text),
      m_copyFun(copyFun),
      m_renewResponseFun(renewResponseFun),
      m_listWidget(listWidget),
      m_thinkTimeLengthList(thinkTimeLengthList),
      m_thinkTimeIndex(thinkTimeIndex),
      m_isUser(isUser),
      m_thinkIsExpand(thinkIsExpand),
      m_textMaxWidth(textMaxWidth)
{
    layout = new QHBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    label = new QLabel(m_text);
    label->setWordWrap(true);
    label->setMaximumWidth(m_textMaxWidth);
    label->adjustSize();
    layout->addWidget(label);
    if (!m_isUser) layout->addStretch();
    else           label->setAlignment(Qt::AlignRight);
    setFixedSize(label->size());
    QTimer::singleShot(50, [this]{emit executeNext();});
}

MessageWidget::~MessageWidget()
{
}

bool MessageWidget::getIsUser()
{
    return m_isUser;
}

QString MessageWidget::getText()
{
    return m_text;
}

void MessageWidget::updateFunWidgetSize(qreal curDpi, qreal initDpi)
{
    Q_UNUSED(curDpi);
    Q_UNUSED(initDpi);
}

void MessageWidget::toggleWidget()
{

}

void MessageWidget::breakHandle()
{

}

void MessageWidget::removeRenewResponseButton()
{

}

void MessageWidget::setText(const QString &text)
{
    label->setText(text);
    label->adjustSize();
    setFixedSize(label->size());
}

void MessageWidget::removeLoadingWidget()
{

}
