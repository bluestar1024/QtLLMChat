#include "copybutton.h"
#include "messagewidget.h"
#include "appcontext.h"

CopyButton::CopyButton(AppContext *appContext, const QString &tipText, int tipOffsetX,
                       int tipOffsetY, QWidget *parent)
    : QPushButton(parent), appContext(appContext), tipText(tipText)
{
    setCursor(Qt::PointingHandCursor);
    tipStartPos = rect().topLeft() - QPoint(tipOffsetX, tipOffsetY);
}

CopyButton::~CopyButton() { }

void CopyButton::mousePressEvent(QMouseEvent *e)
{
    qDebug() << "parent:" << parent() << "qobject_cast<MessageWidget *>(parent()):"
             << qobject_cast<MessageWidget *>(parent());
    QString text;
    // if (auto *messageWidget = qobject_cast<MessageWidget *>(parent())) {
    if (auto *messageWidget = getMessageWidget()) {
        if (messageWidget->hasSelectedText())
            text = messageWidget->getSelectedText();
        else
            text = messageWidget->getText();
        if (!text.isEmpty())
            QApplication::clipboard()->setText(text);
        // QPushButton::mousePressEvent(e);
        // e->ignore();
    }
    QPushButton::mousePressEvent(e);
    e->ignore();
    qDebug() << "复制文本：" << text;
}

void CopyButton::mouseReleaseEvent(QMouseEvent *e)
{
    QPushButton::mouseReleaseEvent(e);
    e->ignore();
}

bool CopyButton::event(QEvent *e)
{
    if (e->type() == QEvent::ToolTip) {
        // 提示字体由 MainWindow::applyToolTipFont 统一设置（像素字号），
        // 此处不可再 setFont(pointSize)，否则提示框尺寸会大于文字所需尺寸
        QToolTip::showText(mapToGlobal(tipStartPos), tipText, this);
    }
    return QPushButton::event(e);
}

MessageWidget *CopyButton::getMessageWidget()
{
    QObject *w = qobject_cast<QObject *>(this);
    do {
        w = w->parent();
        if (!w)
            break;
    } while (!qobject_cast<MessageWidget *>(w));
    return qobject_cast<MessageWidget *>(w);
}
