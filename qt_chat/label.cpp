#include "label.h"
#include "appcontext.h"

Label::Label(AppContext *appContext, QWidget *parent) : QLabel(parent), appContext(appContext)
{
    resize(50, 32);

    // 字体族由 AppContext 统一注册缓存，直接取用（不再逐个 addApplicationFont）
    fontFamily = appContext->fontFamily();
    if (!fontFamily.isEmpty()) {
        QFont font(fontFamily);
        font.setPixelSize(appContext->windowFontPixelSize());
        setFont(font);
    }
}

void Label::updateFontSize()
{
    if (fontFamily.isEmpty())
        return;
    QFont font(fontFamily);
    font.setPixelSize(appContext->windowFontPixelSize());
    setFont(font);
}

Label::~Label() { }
