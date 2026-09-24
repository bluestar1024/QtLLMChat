#ifndef THINKWIDGET_H
#define THINKWIDGET_H

#pragma once
#include "customlabel.h"
#include "webengineview.h"
#include "markdown_parser.h"
#include "html_renderer.h"

#include <QtWidgets/QWidget>
#include <QtGui/QFont>
#include <QtGui/QFontDatabase>
#include <QtGui/QFontMetricsF>
#include <QtWidgets/QHBoxLayout>
#include <QtCore/QTimer>
#include <QtCore/QUrl>
#include <QtCore/QFileInfo>

// extern const QString fontFilePath;
// extern const QString mathjaxScriptPath;
// extern const int windowFontPixelSize;

class AppContext;
class WebEngineView;

class ThinkWidget : public QWidget
{
    Q_OBJECT
public:
    explicit ThinkWidget(AppContext *appContext, const QString &text = "",
                         std::function<void()> sizeFinishFun = nullptr, int maxWidth = 810,
                         QWidget *parent = nullptr);
    ~ThinkWidget();

    void setText(const QString &text);
    QString getText();
    // 旧控件被摘除/即将销毁前调用（新建聊天、切换记录、窗口重建）：
    // 停止尺寸探测定时器、中止页面加载并断开 page 信号，避免其异步回调
    // （loadFinished/contentsSizeChanged/runJavaScript 结果）在控件销毁前
    // 进入后续事件循环（如新会话渲染的嵌套等待）中触发
    void stopPendingWork();
    // void toggleWidget();
    bool hasSelectedText() const;
    QString getSelectedText() const;
    // void setIsEmitSizeFinish(bool flag);
    // bool getIsEmitSizeFinish();
    void setIsSizeFinish(bool flag);
    bool getIsSizeFinish();
    int getMaxWidth() const { return maxWidth; }

protected:
    WebEngineView *webEngineView;
    QHBoxLayout *mainHLayout;
    QSize webEngineSize;
    QTimer *updateSizeTimer;
    bool isSetTextEnd;
    // bool isEmitSizeFinish;
    bool isSizeFinish;
    // 页面加载流程是否已结束（loadFinished 到达，无论成败）：用于区分
    // "加载中尺寸未稳定"与"已加载但内容为空（高度 0）"两种量不到正尺寸的情形
    bool isPageLoaded;
    // 页面加载完成后连续量到无效尺寸（w/h <= 0）的次数：空内容块（如流式
    // 输出中未闭合的 "**" 解析后无 HTML 输出）高度恒为 0，限次后按当前
    // 尺寸收敛，避免渲染等待循环永远等不到 isSizeFinish 而卡死
    int invalidSizeCount;
    bool waitForValidSize();

signals:
    void setSizeFinished();

private slots:
    void onPageLoadFinished(bool success);
    void onContentsSizeChanged(const QSizeF &size);
    virtual void onUpdateSize();

private:
    AppContext *appContext;

    void measureText(const QString &text, int &labelWidth, int &labelHeight) const;
    QString htmlReplaceText(const QString &text) const;
    struct TableInfo
    {
        QString tableText;
        QStringList items;
        QStringList alignList;
        int row = 0;
        int col = 0;
        bool complete = false;
    };
    TableInfo getTable(const QString &text) const;
    QString getAlignmentClass(const QString &fmt) const;

    QString text;
    std::function<void()> sizeFinishFun;
    int maxWidth;
    // bool isLabel;

    CustomLabel *label;
    QFont font;
    QFontMetricsF *fontMetrics;

    QString mathJaxCdn;
    QString htmlText;
    QString fullHtmlText;
};

#endif // THINKWIDGET_H
