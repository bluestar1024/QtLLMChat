
#ifndef APPCONTEXT_H
#define APPCONTEXT_H

#pragma once
#include <QtCore/QString>
#include <QtCore/QPoint>
#include <QtCore/QRect>
#include <QtGui/QColor>

class QScreen;

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

    // 代码主题文件（可读写，CodeEditor 切换主题时写入）
    const QString &codeThemeFilePath() const;
    void setCodeThemeFilePath(const QString &path);

    // 应用字体族名：首次调用时注册字体文件并缓存结果，之后直接复用；
    // 字体不可用时返回空串（调用方应跳过 setFont）。控件在会话切换/窗口重建中
    // 会大量创建，不应各自调用 addApplicationFont —— 重复注册会使 Qt 字体
    // 数据库不断累积，最终 applicationFontFamilies 可能返回空列表引发崩溃
    const QString &fontFamily();

    // 字体（pointSize 只读；pixelSize 随 DPI 变化由 MainWindow 重算写入）
    int windowFontPointSize() const;
    int windowFontPixelSize() const;
    void setWindowFontPixelSize(int size);
    int titleFontPointSize() const;
    int titleFontPixelSize() const;
    void setTitleFontPixelSize(int size);
    int buttonFontPointSize() const;
    // QToolTip 提示文字的字号（像素）：pointSize 字体的度量按应用字体 DPI
    // 解析，提示窗口却按自己所在屏的 DPI 绘制，跨 DPI 屏时两者不一致（本项
    // 目关闭高 DPI 缩放，150% 缩放的屏为 144 DPI、100% 缩放的屏为 96 DPI），
    // 会导致提示框尺寸按 18px/字算、文字却按 12px/字画，出现"框远大于文字"；
    // 像素字号不随 DPI 变化，度量与绘制始终一致
    int buttonFontPixelSize() const;
    // 按提示所在屏的 DPI 应用 QToolTip 字体：字号以像素值与该屏 DPI 换算，
    // 保证度量与绘制使用同一字号。每次显示提示前调用：初始化时窗口可能被
    // 系统放到与主屏 DPI 不同的屏上，只在 DPI 变化时更新会沿用主屏算出的
    // 偏大字号
    void applyToolTipFontForScreen(QScreen *screen);
    // 计算 QToolTip 的显示位置（QToolTip::showText 的 pos 参数）：水平使提
    // 示框中心与按钮中心对齐（相对按钮左右对称），竖直使提示底边位于按钮
    // 顶边上方固定间隙处。QToolTip 内部会把传入位置再偏移 (2, 光标高度)
    // （Windows 光标高度随屏 DPI 变化：96 DPI 为 16、144 DPI 为 24），提示
    // 尺寸也随字号变化，均按实际值补偿，避免高 DPI 屏上提示下探覆盖按钮
    // 与鼠标
    QPoint toolTipPos(const QRect &buttonGlobalRect, const QString &text) const;

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

    // fontFamily() 的懒加载缓存（fontFamilyLoaded_ 标记"已尝试注册"，
    // 字体不可用时 fontFamily_ 保持空串，避免反复重试注册）
    QString fontFamily_;
    bool fontFamilyLoaded_;

    int windowFontPointSize_;
    int windowFontPixelSize_;
    int titleFontPointSize_;
    int titleFontPixelSize_;
    int buttonFontPointSize_;
    int buttonFontPixelSize_;

    QColor textEditFullBGColor_;
    QColor textEditFullBGTColor_;
    QColor textEditFullBColor_;
    QColor textEditFullBTColor_;

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
