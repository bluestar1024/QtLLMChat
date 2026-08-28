
#ifndef APPCONTEXT_H
#define APPCONTEXT_H

#pragma once
#include <QtCore/QString>
#include <QtGui/QColor>

class QWebEngineProfile;

// 应用上下文：收编原 globalvariables.h 中的全部全局变量，
// 由 MainWindow 创建并通过构造函数逐层注入到各控件
class AppContext
{
public:
    AppContext();

    // 资源路径（只读）
    const QString &imagesDir() const;
    const QString &fontFilePath() const;
    const QString &configFilePath() const;
    const QString &mathjaxScriptPath() const;
    const QString &chatRecordsDir() const;
    const QString &webEngineCacheDir() const;

    // 代码主题文件（可读写，CodeEditor 切换主题时写入）
    const QString &codeThemeFilePath() const;
    void setCodeThemeFilePath(const QString &path);

    // WebEngine 共享 profile：指针可写（仅 MainWindow 初始化时写入一次），
    // 内容只读（本类不暴露任何修改 profile 内容的接口）
    QWebEngineProfile *webEngineProfile() const;
    void setWebEngineProfile(QWebEngineProfile *profile);

    // 字体（pointSize 只读；pixelSize 随 DPI 变化由 MainWindow 重算写入）
    int windowFontPointSize() const;
    int windowFontPixelSize() const;
    void setWindowFontPixelSize(int size);
    int titleFontPointSize() const;
    int titleFontPixelSize() const;
    void setTitleFontPixelSize(int size);
    int buttonFontPointSize() const;

    // textEditFull 配色（只读）
    const QColor &textEditFullBGColor() const;
    const QColor &textEditFullBGTColor() const;
    const QColor &textEditFullBTColor() const;
    const QColor &textEditFullBColor() const;

    // LLM 初始配置与参数范围（只读）
    const QString &initBaseUrl() const;
    const QString &initApiKey() const;
    const QString &initModel() const;
    int maxTokensMinimum() const;
    int maxTokensMaximum() const;
    int initMaxTokensCurrentVal() const;
    int topPMinimum() const;
    int topPMaximum() const;
    double initTopPCurrentVal() const;
    double topPSingleStep() const;
    double temperatureMinimum() const;
    int temperatureMaximum() const;
    double initTemperatureCurrentVal() const;
    double temperatureSingleStep() const;

private:
    QString imagesDir_;
    QString fontFilePath_;
    QString configFilePath_;
    QString mathjaxScriptPath_;
    QString chatRecordsDir_;
    QString codeThemeFilePath_;
    QString webEngineCacheDir_;
    QWebEngineProfile *webEngineProfile_;

    int windowFontPointSize_;
    int windowFontPixelSize_;
    int titleFontPointSize_;
    int titleFontPixelSize_;
    int buttonFontPointSize_;

    QColor textEditFullBGColor_;
    QColor textEditFullBGTColor_;
    QColor textEditFullBTColor_;
    QColor textEditFullBColor_;

    QString initBaseUrl_;
    QString initApiKey_;
    QString initModel_;
    int maxTokensMinimum_;
    int maxTokensMaximum_;
    int initMaxTokensCurrentVal_;
    int topPMinimum_;
    int topPMaximum_;
    double initTopPCurrentVal_;
    double topPSingleStep_;
    double temperatureMinimum_;
    int temperatureMaximum_;
    double initTemperatureCurrentVal_;
    double temperatureSingleStep_;
};

#endif // APPCONTEXT_H
