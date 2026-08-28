#include "appcontext.h"

#include <QtWebEngineCore/QWebEngineProfile>

AppContext::AppContext()
    : imagesDir_(":/images"),
      fontFilePath_(":/font/msyhl.ttc"),
      configFilePath_(":/config/config.txt"),
      mathjaxScriptPath_("mathjax/es5/tex-mml-chtml.js"),
      chatRecordsDir_("../../chatrecords/"),
      codeThemeFilePath_(":/config/dark_theme.xml"),
      webEngineCacheDir_("../../webengine_cache"),
      webEngineProfile_(nullptr),
      windowFontPointSize_(10),
      windowFontPixelSize_(20),
      titleFontPointSize_(14),
      titleFontPixelSize_(28),
      buttonFontPointSize_(9),
      textEditFullBGColor_(QColor(224, 224, 224)),
      textEditFullBGTColor_(QColor(224, 224, 224, 0)),
      textEditFullBTColor_(QColor(100, 100, 100)),
      textEditFullBColor_(QColor(100, 100, 100, 0)),
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

const QString &AppContext::webEngineCacheDir() const
{
    return webEngineCacheDir_;
}

const QString &AppContext::codeThemeFilePath() const
{
    return codeThemeFilePath_;
}

void AppContext::setCodeThemeFilePath(const QString &path)
{
    codeThemeFilePath_ = path;
}

QWebEngineProfile *AppContext::webEngineProfile() const
{
    return webEngineProfile_;
}

void AppContext::setWebEngineProfile(QWebEngineProfile *profile)
{
    webEngineProfile_ = profile;
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

const QColor &AppContext::textEditFullBGColor() const
{
    return textEditFullBGColor_;
}

const QColor &AppContext::textEditFullBGTColor() const
{
    return textEditFullBGTColor_;
}

const QColor &AppContext::textEditFullBTColor() const
{
    return textEditFullBTColor_;
}

const QColor &AppContext::textEditFullBColor() const
{
    return textEditFullBColor_;
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
