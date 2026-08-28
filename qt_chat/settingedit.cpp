#include "settingedit.h"
#include "appcontext.h"

SettingEdit::SettingEdit(AppContext *appContext, QWidget *parent)
    : QLineEdit(parent), appContext(appContext)
{
    resize(282, 32);
    updateStyleSheet();
}

SettingEdit::~SettingEdit() { }

void SettingEdit::setSize(QSize s)
{
    setFixedSize(s);
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
