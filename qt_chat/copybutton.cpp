#include "copybutton.h"

CopyButton::CopyButton(const QString &tipText, int tipOffsetX, int tipOffsetY, QWidget *parent)
    : QPushButton(parent), tipText(tipText)
{
    setCursor(Qt::PointingHandCursor);
    tipStartPos = rect().topLeft() - QPoint(tipOffsetX, tipOffsetY);
}

CopyButton::~CopyButton() { }

void CopyButton::mousePressEvent(QMouseEvent *e)
{
    QString text;
    if (auto *messageWidget = qobject_cast<MessageWidget *>(parent())) {
        if (messageWidget->hasSelectedText())
            text = messageWidget->getSelectedText();
        else
            text = messageWidget->getText();
        if (!text.isEmpty())
            QApplication::clipboard()->setText(text);
        QPushButton::mousePressEvent(e);
        e->ignore();
    }
}

void CopyButton::mouseReleaseEvent(QMouseEvent *e)
{
    QPushButton::mouseReleaseEvent(e);
    e->ignore();
}

bool CopyButton::event(QEvent *e)
{
    if (e->type() == QEvent::ToolTip) {
        static int fontId = -1;
        static QString families;
        if (fontId == -1) {
            fontId = QFontDatabase::addApplicationFont(fontFilePath);
            if (fontId != -1) {
                families = QFontDatabase::applicationFontFamilies(fontId).value(0);
            }
        }
        if (!families.isEmpty()) {
            QFont font(families, buttonFontPointSize);
            QToolTip::setFont(font);
        }
        QToolTip::showText(mapToGlobal(tipStartPos), tipText, this);
    }
    return QPushButton::event(e);
}
