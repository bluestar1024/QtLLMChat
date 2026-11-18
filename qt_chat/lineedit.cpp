#include "lineedit.h"
#include "appcontext.h"
#include "dpiutil.h"

#include <QtWidgets/QApplication>
#include <QtGui/QScreen>
#include <QtCore/QDir>

LineEdit::LineEdit(AppContext *appContext, QWidget *parent)
    : QLineEdit(parent), appContext(appContext), appliedDpi(DpiUtil::kBaseDpi)
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

    widgetSizeDict["self"] = DpiUtil::toBaseSize(size(), appliedDpi);
    widgetSizeDict["searchButton"] = DpiUtil::toBaseSize(searchButton->size(), appliedDpi);
    widgetSizeDict["searchButton iconSize"] =
            DpiUtil::toBaseSize(searchButton->iconSize(), appliedDpi);
}

LineEdit::~LineEdit() { }

void LineEdit::resetWidgetSize()
{
    // 宽度随容器布局变化：由调用方（ChatRecordsWidget::setSize）先行 setFixedSize，
    // 这里把当前像素尺寸按已应用 DPI 折算回 144 基准
    widgetSizeDict["self"] = DpiUtil::toBaseSize(size(), appliedDpi);
}

void LineEdit::updateSize(qreal curDpi, qreal lastDpi)
{
    // 缓存值均为 144 DPI 基准：按目标 DPI 绝对换算（不再用 curDpi/lastDpi 叠乘，
    // 也不再把当前像素值回写为下次的基准，避免取整误差逐次累积）
    Q_UNUSED(lastDpi);
    if (curDpi <= 0)
        curDpi = DpiUtil::kBaseDpi;

    setFixedSize(DpiUtil::scaleSize(widgetSizeDict["self"], curDpi));
    searchButton->setFixedSize(DpiUtil::scaleSize(widgetSizeDict["searchButton"], curDpi));
    searchButton->setIconSize(
            DpiUtil::scaleSize(widgetSizeDict["searchButton iconSize"], curDpi));

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

    appliedDpi = curDpi;
}
