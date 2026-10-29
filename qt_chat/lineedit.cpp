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

    // 字体族由 AppContext 统一注册缓存，直接取用（不再逐个 addApplicationFont）
    const QString fontFamily = appContext->fontFamily();
    if (!fontFamily.isEmpty()) {
        font = QFont(fontFamily);
        font.setPixelSize(appContext->windowFontPixelSize());
        setFont(font);
    }

    searchButton = new PushButton(appContext, "搜索", this);
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

void LineEdit::updateSize(qreal curDpi, qreal lastDpi)
{
    QSize selfSize = widgetSizeDict["self"];
    int newWidth = qRound(selfSize.width() * curDpi / lastDpi);
    int newHeight = qRound(selfSize.height() * curDpi / lastDpi);
    setFixedSize(newWidth, newHeight);

    QSize buttonSize = widgetSizeDict["searchButton"];
    int btnNewWidth = qRound(buttonSize.width() * curDpi / lastDpi);
    int btnNewHeight = qRound(buttonSize.height() * curDpi / lastDpi);
    searchButton->setFixedSize(btnNewWidth, btnNewHeight);

    QSize buttonIconSize = widgetSizeDict["searchButton iconSize"];
    int btnIconNewWidth = qRound(buttonIconSize.width() * curDpi / lastDpi);
    int btnIconNewHeight = qRound(buttonIconSize.height() * curDpi / lastDpi);
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
