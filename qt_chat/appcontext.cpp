#include "appcontext.h"

#include <QtGui/QFontDatabase>
#include <QtGui/QFontMetrics>
#include <QtGui/QGuiApplication>
#include <QtGui/QScreen>
#include <QtWidgets/QApplication>
#include <QtWidgets/QToolTip>

#include <cmath>

AppContext::AppContext()
    : imagesDir_(":/images"),
      fontFilePath_(":/font/msyhl.ttc"),
      configFilePath_(":/config/config.txt"),
      mathjaxScriptPath_("mathjax/es5/tex-mml-chtml.js"),
      chatRecordsDir_("../../chatrecords/"),
      codeThemeFilePath_(":/config/dark_theme.xml"),
      fontFamilyLoaded_(false),
      windowFontPointSize_(10),
      windowFontPixelSize_(20),
      titleFontPointSize_(14),
      titleFontPixelSize_(28),
      buttonFontPointSize_(9),
      buttonFontPixelSize_(18),
      textEditFullBGColor_(QColor(224, 224, 224)),
      textEditFullBGTColor_(QColor(224, 224, 224, 0)),
      textEditFullBColor_(QColor(100, 100, 100)),
      textEditFullBTColor_(QColor(100, 100, 100, 0)),
      initBaseUrl_("http://127.0.0.1:11434/v1"),
      initApiKey_("EMPTY"),
      initModel_("deepseek-r1:1.5b"),
      maxTokensMinimum_(0),
      maxTokensMaximum_(32768),
      initMaxTokensCurrentVal_(5000),
      topPMinimum_(0),
      topPMaximum_(1),
      initTopPCurrentVal_(0.8),
      topPSingleStep_(0.01),
      temperatureMinimum_(0.01),
      temperatureMaximum_(1),
      initTemperatureCurrentVal_(0.8),
      temperatureSingleStep_(0.01)
{
}

const QString &AppContext::imagesDir() const
{
    return imagesDir_;
}

const QString &AppContext::fontFilePath() const
{
    return fontFilePath_;
}

const QString &AppContext::fontFamily()
{
    // 只在首次调用时注册字体：逐控件注册会使 Qt 字体数据库重复累积
    if (!fontFamilyLoaded_) {
        fontFamilyLoaded_ = true;
        int fontId = QFontDatabase::addApplicationFont(fontFilePath_);
        if (fontId != -1) {
            // applicationFontFamilies 可能返回空列表（字体重复注册、字体数据库
            // 重建等情况），取用时必须判空，否则 at(0) 越界读取直接崩溃
            const QStringList families = QFontDatabase::applicationFontFamilies(fontId);
            if (!families.isEmpty())
                fontFamily_ = families.at(0);
        }
    }
    return fontFamily_;
}

const QString &AppContext::configFilePath() const
{
    return configFilePath_;
}

const QString &AppContext::mathjaxScriptPath() const
{
    return mathjaxScriptPath_;
}

const QString &AppContext::chatRecordsDir() const
{
    return chatRecordsDir_;
}

const QString &AppContext::codeThemeFilePath() const
{
    return codeThemeFilePath_;
}

void AppContext::setCodeThemeFilePath(const QString &path)
{
    codeThemeFilePath_ = path;
}

int AppContext::windowFontPointSize() const
{
    return windowFontPointSize_;
}

int AppContext::windowFontPixelSize() const
{
    return windowFontPixelSize_;
}

void AppContext::setWindowFontPixelSize(int size)
{
    windowFontPixelSize_ = size;
}

int AppContext::titleFontPointSize() const
{
    return titleFontPointSize_;
}

int AppContext::titleFontPixelSize() const
{
    return titleFontPixelSize_;
}

void AppContext::setTitleFontPixelSize(int size)
{
    titleFontPixelSize_ = size;
}

int AppContext::buttonFontPointSize() const
{
    return buttonFontPointSize_;
}

int AppContext::buttonFontPixelSize() const
{
    return buttonFontPixelSize_;
}

void AppContext::applyToolTipFontForScreen(QScreen *screen)
{
    // 字号必须按提示窗口所在屏的 DPI 换算：pointSize 是磅值，换算成像素要
    // 乘 DPI——QFontMetrics/QLabel::sizeHint 度量时用字体对象自身的 DPI 基准
    // （QFont::dpi()，默认跟随应用字体所在屏，本程序启动时主屏为 144），
    // 提示窗口绘制时却按窗口实际所在屏的 DPI（副屏为 96）：同一 9pt 字体
    // 一边按 18px/字算框体、一边按 12px/字画文字，框远大于文字；像素字号
    // 不随 DPI 变化，度量与绘制始终一致，提示框尺寸恰好贴合文字
    const qreal dpi = screen ? screen->logicalDotsPerInch() : 96.0;
    buttonFontPixelSize_ = std::ceil(buttonFontPointSize_ * dpi / 72);
    QFont font = QApplication::font();
    font.setPixelSize(buttonFontPixelSize_);
    QToolTip::setFont(font);
}

QPoint AppContext::toolTipPos(const QRect &buttonGlobalRect, const QString &text) const
{
    // 提示尺寸：QTipLabel 是 QLabel（AlignLeft、margin=1、indent=1），并应用
    // 了 QToolTip 样式表，宽度 = 文本 advance + 11（两倍 margin + indent + 样式表
    // 内容区，实测 96/144 DPI 下均为 11），高度 = boundingRect 高 + 2*margin
    QFontMetrics fm(QToolTip::font());
    const QRect textRect = fm.boundingRect(0, 0, -1, 2000,
                                           Qt::AlignLeft | Qt::TextSingleLine, text);
    const int tipWidth = fm.horizontalAdvance(text) + 11;
    const int tipHeight = textRect.height() + 2;
    // 光标高度：Windows 默认箭头光标高 16px@96dpi，随屏 DPI 线性缩放
    const QScreen *tipScreen = QGuiApplication::screenAt(buttonGlobalRect.topLeft());
    const qreal dpi = tipScreen ? tipScreen->logicalDotsPerInch() : 96.0;
    const int cursorHeight = qRound(16.0 * dpi / 96.0);
    // 提示底边与按钮顶边的间隙
    constexpr int gap = 2;
    // 水平：QToolTip 内部会把传入位置再偏移 2，先减去该偏移，再让提示框
    // 水平中心与按钮水平中心重合，使提示相对按钮左右对称
    const int centerX = buttonGlobalRect.left() + buttonGlobalRect.width() / 2;
    const int x = centerX - tipWidth / 2 - 2;
    // 竖直：补偿光标高度后提示底边落在按钮顶边上方 gap 处
    const int y = buttonGlobalRect.top() - (cursorHeight + tipHeight + gap);
    return QPoint(x, y);
}

const QColor &AppContext::textEditFullBGColor() const
{
    return textEditFullBGColor_;
}

const QColor &AppContext::textEditFullBGTColor() const
{
    return textEditFullBGTColor_;
}

const QColor &AppContext::textEditFullBColor() const
{
    return textEditFullBColor_;
}

const QColor &AppContext::textEditFullBTColor() const
{
    return textEditFullBTColor_;
}

const QString &AppContext::initBaseUrl() const
{
    return initBaseUrl_;
}

const QString &AppContext::initApiKey() const
{
    return initApiKey_;
}

const QString &AppContext::initModel() const
{
    return initModel_;
}

int AppContext::maxTokensMinimum() const
{
    return maxTokensMinimum_;
}

int AppContext::maxTokensMaximum() const
{
    return maxTokensMaximum_;
}

int AppContext::initMaxTokensCurrentVal() const
{
    return initMaxTokensCurrentVal_;
}

int AppContext::topPMinimum() const
{
    return topPMinimum_;
}

int AppContext::topPMaximum() const
{
    return topPMaximum_;
}

double AppContext::initTopPCurrentVal() const
{
    return initTopPCurrentVal_;
}

double AppContext::topPSingleStep() const
{
    return topPSingleStep_;
}

double AppContext::temperatureMinimum() const
{
    return temperatureMinimum_;
}

int AppContext::temperatureMaximum() const
{
    return temperatureMaximum_;
}

double AppContext::initTemperatureCurrentVal() const
{
    return initTemperatureCurrentVal_;
}

double AppContext::temperatureSingleStep() const
{
    return temperatureSingleStep_;
}
