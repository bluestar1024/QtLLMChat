#include "messagewidget.h"
#include <QTimer>

MessageWidget::MessageWidget(const QString &text, std::function<void()> copyFun,
                             std::function<void()> renewResponseFun, ListWidget *listWidget,
                             QList<int> &thinkTimeLengthList, int thinkTimeIndex, bool isUser,
                             bool thinkIsExpand, int textMaxWidth, QWidget *parent)
    : QWidget(parent),
      m_text(text),
      m_copyFun(copyFun),
      m_renewResponseFun(renewResponseFun),
      m_listWidget(listWidget),
      m_thinkTimeLengthList(thinkTimeLengthList),
      m_thinkTimeIndex(thinkTimeIndex),
      m_isUser(isUser),
      m_thinkIsExpand(thinkIsExpand),
      m_textMaxWidth(textMaxWidth - 10)
{
    m_layout = new QHBoxLayout(this);
    m_layout->setContentsMargins(5, 5, 5, 5);
    m_textShow = new TextShow(m_text, m_isUser, m_textMaxWidth, this);
    m_layout->addWidget(m_textShow);
    // if (!m_isUser) m_layout->addStretch();
    // else           m_layout->setAlignment(Qt::AlignRight);
    setFixedSize(m_textShow->size() + QSize(10, 0));
    setObjectName("messageWidget");
    setStyleSheet("#messageWidget{"
                  "background: green;"
                  "}");
}

MessageWidget::~MessageWidget() { }

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
    connect(m_textShow, &TextShow::setSizeFinished, this, &MessageWidget::onSizeFinshed);
    m_textShow->toggleWidget();
}

void MessageWidget::breakHandle() { }

void MessageWidget::removeRenewResponseButton() { }

void MessageWidget::setSize()
{
    setFixedSize(m_textShow->size() + QSize(10, 0));
}

void MessageWidget::setText(const QString &text)
{
    m_text = text;
    m_textShow->setText(m_text);
    setFixedSize(m_textShow->size() + QSize(10, 0));
}

void MessageWidget::removeLoadingWidget() { }

void MessageWidget::onSizeFinshed()
{
    emit resizeFinished();
    if (!m_isUser)
        emit setTexting(false);
}

ListWidget *MessageWidget::getListWidget()
{
    return m_listWidget;
}
