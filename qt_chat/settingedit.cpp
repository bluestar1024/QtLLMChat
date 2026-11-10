#include "settingedit.h"
#include "appcontext.h"

SettingEdit::SettingEdit(AppContext *appContext, QWidget *parent)
    : QLineEdit(parent), appContext(appContext)
{
    resize(282, 32);
    // 高度固定、宽度随行布局分配：宽度交给 QHBoxLayout，能保证右边缘始终
    // 落在数据行内部，不会被父部件裁掉
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    setFixedHeight(32);
    updateStyleSheet();
}

SettingEdit::~SettingEdit() { }

void SettingEdit::setHeight(int h)
{
    setFixedHeight(h);
    updateStyleSheet();
}

void SettingEdit::updateFontSize()
{
    updateStyleSheet();
}

void SettingEdit::updateStyleSheet()
{
    setStyleSheet(QString(R"(
        QLineEdit {
            border-left: 1px solid #e4e4e4;
            border-top: 1px solid #e4e4e4;
            border-right: 1px solid black;
            border-bottom: 1px solid black;
            background: transparent;
            font-size: %1px;
        }
    )")
                          .arg(appContext->windowFontPixelSize()));
}
