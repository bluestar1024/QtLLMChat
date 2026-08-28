#include "label.h"
#include "appcontext.h"

Label::Label(AppContext *appContext, QWidget *parent) : QLabel(parent), appContext(appContext)
{
    resize(50, 32);

    int fontId = QFontDatabase::addApplicationFont(appContext->fontFilePath());
    if (fontId != -1) {
        QStringList fontFamilies = QFontDatabase::applicationFontFamilies(fontId);
        if (!fontFamilies.isEmpty()) {
            QString fontFamily = fontFamilies.at(0);
            QFont font(fontFamily);
            font.setPixelSize(appContext->windowFontPixelSize());
            setFont(font);
        }
    }
}

Label::~Label() { }
