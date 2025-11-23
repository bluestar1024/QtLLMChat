#include "sendbutton.h"

SendButton::SendButton(const QString &tipText,
                       int tipOffsetX,
                       int tipOffsetY,
                       QWidget *parent)
    : QPushButton(parent),
      m_tipText(tipText),
      m_ox(tipOffsetX),
      m_oy(tipOffsetY)
{
    setCursor(Qt::PointingHandCursor);
    m_tipStartPos = rect().topLeft() - QPoint(m_ox, m_oy);
}

SendButton::~SendButton()
{
}

bool SendButton::event(QEvent *event)
{
    if (event->type() == QEvent::ToolTip) {
        static bool fontLoaded = false;
        static QFont customFont;
        if (!fontLoaded) {
            int fontId = QFontDatabase::addApplicationFont(fontFilePath);
            if (fontId != -1) {
                QStringList families = QFontDatabase::applicationFontFamilies(fontId);
                if (!families.isEmpty()) {
                    customFont = QFont(families.at(0), buttonFontPointSize);
                    fontLoaded = true;
                }
            }
        }
        if (fontLoaded)
            QToolTip::setFont(customFont);
        QToolTip::showText(mapToGlobal(m_tipStartPos), m_tipText, this);
    }
    return QPushButton::event(event);
}
