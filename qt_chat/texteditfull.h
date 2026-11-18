#ifndef TEXTEDITFULL_H
#define TEXTEDITFULL_H

#pragma once
#include "textedit.h"

#include <QtWidgets/QWidget>
#include <QtWidgets/QHBoxLayout>
#include <QtCore/QPropertyAnimation>
#include <QtGui/QMouseEvent>
#include <QtGui/QPaintEvent>
#include <QtGui/QPainter>
#include <QtGui/QPen>
#include <QtGui/QBrush>
#include <QtGui/QPainterPath>
#include <QtGui/QColor>
#include <QtCore/QMetaMethod>

// extern const QColor textEditFullBGColor;
// extern const QColor textEditFullBGTColor;
// extern const QColor textEditFullBTColor;
// extern const QColor textEditFullBColor;

class AppContext;

class TextEditFull : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(QColor backgroundColor READ getBackgroundColor WRITE setBackgroundColor)
    Q_PROPERTY(QColor borderColor READ getBorderColor WRITE setBorderColor)

public:
    explicit TextEditFull(AppContext *appContext, QWidget *parent = nullptr);
    ~TextEditFull();

    void backgroundColorShowLight();
    void backgroundColorShowDark();
    void clearFocus();
    // 按新屏 DPI 重设 chatInput 内各部件尺寸（以 144 DPI 屏的物理尺寸为基准
    // 按 curDpi / 144 绝对换算，与 lastDpi 无关）：反复跨屏不会因逐次取整
    // 而漂移；边缘宽度（15px 内边距）与边缘圆角（16px）也参与缩放，1px 描边
    // 内缩不缩放。lastDpi 仅为与其它部件的 updateSize 接口保持一致
    void updateSize(qreal curDpi, qreal lastDpi);
    void resetWidgetSize();
    // 高度建议/最小建议均以 144 DPI 屏为基准按屏 DPI 换算：chatInputWidget 在
    // splitter 中的高度取自本控件的 sizeHint（QAbstractScrollArea 的 192px 是
    // 固定像素、不随屏 DPI 变化），不重写则低 DPI 屏上输入框物理偏高
    QSize sizeHint() const override;
    QSize minimumSizeHint() const override;
    // chatInput 在 144 DPI 屏（基准）下的默认高度 = 220，推导：
    //   1) 内部 TextEdit 的默认高度由项目约定为 TextEdit::kBaseTextHeight
    //      （190，TextEdit::sizeHint 返回该值，而不采用 QAbstractScrollArea
    //      默认的固定 192）；
    //   2) chatInput 在 144 DPI 屏的主布局上下内边距各 15（kBaseEdgeMargin）；
    //   3) 190 + 15 + 15 = 220。
    // 边缘宽度也参与缩放，所以 220 * scale 与内部 190 * scale 严格等比：
    // 96 DPI 下 chatInput 147、文本区 127（= 147 - 2 * 10）。该基准值存在
    // texteditfull.cpp 的匿名 namespace（kBaseHeight），MainWindow 分配 splitter
    // 中 chatInputWidget 的高度时以本方法取到它作为物理高度基准
    static int baseHeightPx();
    // 当前 DPI 下的边缘宽度与边缘圆角（像素，均由 144 基准换算）
    int edgeMarginPx() const { return edgeMargin; }
    int edgeRadiusPx() const { return edgeRadius; }
    QString toPlainText() const;
    void clearText();
    template <typename T>
    void connectSendButtonClick(T *receiver, void (T::*slot)());
    template <typename T>
    void connectSendButtonClick(T *receiver, void (T::*slot)(bool));
    void setSending(bool isSending);

protected:
    void mouseMoveEvent(QMouseEvent *e) override;
    bool event(QEvent *e) override;
    void paintEvent(QPaintEvent *e) override;
    void resizeEvent(QResizeEvent *e) override;

private:
    // 按屏 DPI 重设最小高度并驱动 TextEdit 同步换算各内部部件；
    // dpi <= 0 时取控件当前所在屏的 DPI
    void applyDpiScale(qreal dpi);

    AppContext *appContext;
    TextEdit *textEdit;
    QHBoxLayout *mainHLayout;
    QPropertyAnimation *animationBackgroundColor;
    QPropertyAnimation *animationBorderColor;
    QColor bgColor;
    QColor bColor;
    bool backgroundColorIsLight;
    // 已应用的屏 DPI（取整）：与目标 DPI 相同即跳过（首次显示/换屏兜底与
    // MainWindow 的 DPI 缩放链都会调用 applyDpiScale）
    int appliedDpi;
    // 已应用的字体像素字号：换屏自愈会先用旧字号按新 DPI 换算一次，随后
    // MainWindow 更新 AppContext 字号再调 updateSize，此时 DPI 未变但字号
    // 已变，只看 appliedDpi 会把内部字体与占位符停在旧字号
    int appliedFontPixelSize;
    // DPI 换算后等下一次 resize 打印一行高度诊断（不在每次 resize 时刷日志）
    bool logNextResize;
    // 当前 DPI 下的边缘宽度（主布局内边距）与边缘圆角：由 144 基准按屏 DPI
    // 换算，resetWidgetSize 与 paintEvent 使用
    int edgeMargin;
    int edgeRadius;

    QColor getBackgroundColor() const;
    void setBackgroundColor(const QColor &color);
    QColor getBorderColor() const;
    void setBorderColor(const QColor &color);
};

template <typename T>
void TextEditFull::connectSendButtonClick(T *receiver, void (T::*slot)())
{
    textEdit->connectSendButtonClick(receiver, slot);
}

template <typename T>
void TextEditFull::connectSendButtonClick(T *receiver, void (T::*slot)(bool))
{
    textEdit->connectSendButtonClick(receiver, slot);
}

#endif // TEXTEDITFULL_H
