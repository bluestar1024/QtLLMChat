#include "appcontext.h"

#include <QtCore/QDir>
#include <QtCore/QFile>
#include <QtCore/QStringList>
#include <QtGui/QFontDatabase>
#include <QtGui/QFontMetrics>
#include <QtGui/QGuiApplication>
#include <QtGui/QScreen>
#include <QtWidgets/QApplication>
#include <QtWidgets/QToolTip>

#include <cmath>

// 内置配置模板：编译进 exe 的 qrc 资源，只读，不能作为运行时配置文件
static const char *kDefaultConfigPath = ":/config/config.txt";

// 项目唯一可读写配置文件的所在目录（qt_chat/config），由 qt_chat.pro 以源码树
// 绝对路径注入，因此与工作目录、构建输出目录无关。
#ifndef QT_CHAT_CONFIG_DIR
#define QT_CHAT_CONFIG_DIR ""
#endif

// 用资源内容生成可写配置文件。
// 注意不能用 QFile::copy：它会把源文件的权限一并复制，而 qrc 资源是只读的，
// 生成的目标文件会带上只读属性，之后所有写入都会失败。
static bool createWritableConfigFile(const QString &targetPath, const QString &templatePath)
{
    QFile resource(templatePath);
    if (!resource.open(QIODevice::ReadOnly))
        return false;
    const QByteArray content = resource.readAll();
    resource.close();

    QFile file(targetPath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate))
        return false;
    const bool ok = (file.write(content) == content.size());
    file.close();
    return ok;
}

// 以 Append 方式试开：只验证可写，不会截断已有内容
static bool isWritableFile(const QString &path)
{
    QFile probe(path);
    if (!probe.open(QIODevice::WriteOnly | QIODevice::Append))
        return false;
    probe.close();
    return true;
}

AppContext::AppContext()
    : imagesDir_(":/images"),
      fontFilePath_(":/font/msyhl.ttc"),
      configFilePath_(),
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
    initConfigFilePath();
}

// 配置文件只有一个：qt_chat/config/config.txt（项目源码树内，可读可写）。
// qrc 里的 :/config/config.txt 编译进 exe 后只读，对资源路径用
// QIODevice::WriteOnly 打开必然失败（SettingWidget 每次改值都报"无法写入
// 文件"），所以资源只作为该文件缺失时的初始内容来源
void AppContext::initConfigFilePath()
{
    const QString configDir = QString::fromUtf8(QT_CHAT_CONFIG_DIR);
    const QString defaultConfigPath = QString::fromLatin1(kDefaultConfigPath);

    if (configDir.isEmpty()) {
        qDebug() << "错误：未定义 QT_CHAT_CONFIG_DIR（qt_chat.pro 未生效），配置将不会保存";
        configFilePath_ = defaultConfigPath;
        return;
    }

    // config 目录属于项目源码树，缺失时（如误删）直接以资源内容重建
    if (!QDir().mkpath(configDir)) {
        qDebug() << "错误：无法创建配置目录" << configDir;
        configFilePath_ = defaultConfigPath;
        return;
    }

    const QString target = QDir(configDir).filePath("config.txt");

    if (!QFile::exists(target))
        createWritableConfigFile(target, defaultConfigPath);

    // 文件带只读属性时（例如被版本库只读检出）先清掉，否则写入必然失败
    if (QFile::exists(target) && !isWritableFile(target))
        QFile::setPermissions(target, QFileDevice::ReadOwner | QFileDevice::WriteOwner
                                             | QFileDevice::ReadUser
                                             | QFileDevice::WriteUser);

    if (QFile::exists(target) && isWritableFile(target)) {
        configFilePath_ = target;
        qDebug() << "配置文件：" << configFilePath_;
        return;
    }

    // 实在不可写时退到只读资源：界面仍可读到默认值，但设置无法保存
    qDebug() << "错误：配置文件不可写" << target;
    configFilePath_ = defaultConfigPath;
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
