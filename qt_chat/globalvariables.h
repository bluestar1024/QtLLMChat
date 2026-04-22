#ifndef GLOBALVARIABLES_H
#define GLOBALVARIABLES_H

#pragma once
#include <QtCore/QString>
#include <QtGui/QColor>
#include <QtWebEngineCore/QWebEngineProfile>

extern const QString imagesDir;
extern const QString fontFilePath;
extern const QString configFilePath;
extern const QString mathjaxScriptPath;
extern QString codeThemeFilePath;
extern const QString webEngineCacheDir;
extern QWebEngineProfile *sharedProfile;
extern const int windowFontPointSize;
extern int windowFontPixelSize;
extern const int titleFontPointSize;
extern int titleFontPixelSize;
extern const int buttonFontPointSize;
extern const QColor textEditFullBGColor;
extern const QColor textEditFullBGTColor;
extern const QColor textEditFullBTColor;
extern const QColor textEditFullBColor;
extern const QString testText1;
extern const QString testText;
extern const QString testText_code;
extern const QString testText_markdown;

extern const QString initBaseUrl;
extern const QString initApiKey;
extern const QString initModel;
extern const int maxTokensMinimum;
extern const int maxTokensMaximum;
extern const int initMaxTokensCurrentVal;
extern const int topPMinimum;
extern const int topPMaximum;
extern const double initTopPCurrentVal;
extern const double topPSingleStep;
extern const double temperatureMinimum;
extern const int temperatureMaximum;
extern const double initTemperatureCurrentVal;
extern const double temperatureSingleStep;

#endif // GLOBALVARIABLES_H
