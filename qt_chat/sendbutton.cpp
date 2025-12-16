#include "sendbutton.h"

SendButton::SendButton(const QString &tipText, int tipOffsetX, int tipOffsetY, QWidget *parent)
    : QPushButton(parent), tipText(tipText), tipOffsetX(tipOffsetX), tipOffsetY(tipOffsetY)
{
    setCursor(Qt::PointingHandCursor);
    tipStartPos = rect().topLeft() - QPoint(this->tipOffsetX, this->tipOffsetY);
}

SendButton::~SendButton() { }

bool SendButton::event(QEvent *e)
{
    if (e->type() == QEvent::ToolTip) {
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
        QToolTip::showText(mapToGlobal(tipStartPos), tipText, this);
    }
    return QPushButton::event(e);
}
