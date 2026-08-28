#include "lineedit.h"
#include "appcontext.h"

#include <QtWidgets/QApplication>
#include <QtGui/QScreen>
#include <QtCore/QDir>

LineEdit::LineEdit(AppContext *appContext, QWidget *parent)
    : QLineEdit(parent), appContext(appContext)
{
    setFixedSize(1200 / 3 - 82, 32);
    setPlaceholderText(QStringLiteral("输入搜索词"));

    int fontId = QFontDatabase::addApplicationFont(appContext->fontFilePath());
    if (fontId != -1) {
        QStringList fontFamilies = QFontDatabase::applicationFontFamilies(fontId);
        if (!fontFamilies.isEmpty()) {
            QString fontFamily = fontFamilies.at(0);
            font = QFont(fontFamily);
            font.setPixelSize(appContext->windowFontPixelSize());
            setFont(font);
        }
    }

    searchButton = new PushButton(appContext, "搜索", 5, 35, this);
    searchButton->setFixedSize(32, 32);
    QString searchImagePath = appContext->imagesDir() + "/search.png";
    searchButton->setIcon(QIcon(searchImagePath));
    searchButton->setIconSize(QSize(30, 30));
    searchButton->move(0, 0);

    setStyleSheet(R"(
        QPushButton {
            border: none;
        }
        QLineEdit {
            border: none;
            border-radius: 5px;
            background: #b8b8b8;
            padding-left: 32px;
        }
    )");

    widgetSizeDict["self"] = size();
    widgetSizeDict["searchButton"] = searchButton->size();
    widgetSizeDict["searchButton iconSize"] = searchButton->iconSize();
}

LineEdit::~LineEdit() { }

void LineEdit::resetWidgetSize()
{
    widgetSizeDict["self"] = size();
}

void LineEdit::updateSize(int curDpi, int lastDpi)
{
    QSize selfSize = widgetSizeDict["self"];
    int newWidth = qRound(selfSize.width() * curDpi / static_cast<qreal>(lastDpi));
    int newHeight = qRound(selfSize.height() * curDpi / static_cast<qreal>(lastDpi));
    setFixedSize(newWidth, newHeight);

    QSize buttonSize = widgetSizeDict["searchButton"];
    int btnNewWidth = qRound(buttonSize.width() * curDpi / static_cast<qreal>(lastDpi));
    int btnNewHeight = qRound(buttonSize.height() * curDpi / static_cast<qreal>(lastDpi));
    searchButton->setFixedSize(btnNewWidth, btnNewHeight);

    QSize buttonIconSize = widgetSizeDict["searchButton iconSize"];
    int btnIconNewWidth = qRound(buttonIconSize.width() * curDpi / static_cast<qreal>(lastDpi));
    int btnIconNewHeight = qRound(buttonIconSize.height() * curDpi / static_cast<qreal>(lastDpi));
    searchButton->setIconSize(QSize(btnIconNewWidth, btnIconNewHeight));

    if (!font.family().isEmpty()) {
        font.setPixelSize(appContext->windowFontPixelSize());
        setFont(font);
    }

    setStyleSheet(QString(R"(
        QPushButton {
            border: none;
        }
        QLineEdit {
            border: none;
            border-radius: 5px;
            background: #b8b8b8;
            padding-left: %1px;
        }
    )")
                          .arg(searchButton->width()));

    widgetSizeDict["self"] = size();
    widgetSizeDict["searchButton"] = searchButton->size();
    widgetSizeDict["searchButton iconSize"] = searchButton->iconSize();
}
