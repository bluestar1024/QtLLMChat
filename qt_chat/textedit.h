#ifndef TEXTEDIT_H
#define TEXTEDIT_H

#pragma once
#include "sendbutton.h"
#include "custommenu.h"

#include <QtWidgets/QTextEdit>
#include <QtWidgets/QSizePolicy>
#include <QtGui/QFont>
#include <QtWidgets/QGridLayout>
#include <QtGui/QContextMenuEvent>
#include <QtWidgets/QMenu>
#include <QtGui/QAction>
#include <QtGui/QKeySequence>
#include <QtGui/QIcon>
#include <QtGui/QMouseEvent>
#include <QtGui/QKeyEvent>
#include <QtCore/QMetaMethod>

// extern const QString imagesDir;
// extern const QString fontFilePath;
// extern const int windowFontPointSize;
// extern const int windowFontPixelSize;

class AppContext;

class TextEdit : public QTextEdit
{
    Q_OBJECT

public:
    explicit TextEdit(AppContext *appContext, QWidget *parent = nullptr);
    ~TextEdit();

    // 文本区在 144 DPI 基准下的默认高度（项目约定 190，不采用
    // QAbstractScrollArea 默认的固定 192）：TextEditFull 的默认高度
    // 220 = 190 + 2 * 15 边缘，两者在各 DPI 下按 dpi / 144 同步换算。
    // 需要高度基准的其它模块（TextEditFull 的 kBaseHeight）直接引用本常量
    static constexpr int kBaseTextHeight = 190;

    // 按屏 DPI 重设输入框内各部件尺寸：发送按钮（尺寸与图标）、内部布局边距、
    // 滚动条宽度与输入框字体。各项均以 144 DPI 屏的物理尺寸为基准按
    // dpi / 144 绝对换算（不再按上一次 DPI 的比例叠加，反复跨屏不会因逐次
    // 取整而漂移）；dpi <= 0 时取控件当前所在屏的 DPI
    void applyDpiScale(qreal dpi = 0);
    // 输入框默认高度按项目约定返回 190（而不是 QAbstractScrollArea 的固定
    // 192）：TextEditFull 的默认高度 220 = 190 + 2 * 15 边缘，两者保持自洽
    QSize sizeHint() const override;
    template <typename T>
    void connectSendButtonClick(T *receiver, void (T::*slot)());
    template <typename T>
    void connectSendButtonClick(T *receiver, void (T::*slot)(bool));
    void emitSendButtonClicked();
    void setSending(bool isSending);

protected:
    void contextMenuEvent(QContextMenuEvent *e) override;
    void mouseMoveEvent(QMouseEvent *e) override;
    void mousePressEvent(QMouseEvent *e) override;
    void mouseReleaseEvent(QMouseEvent *e) override;
    void keyPressEvent(QKeyEvent *e) override;

private slots:
    void sendButtonShow();

private:
    // 输入框自身的样式表（滚动条宽度随 DPI 换算，QSS 的 px 不随屏 DPI 变化）
    void applyStyleSheet(int scrollBarWidth);

    AppContext *appContext;
    QFont font;
    SendButton *sendButton;
    QGridLayout *gLayout;
    QString sendImagesPath;
    QString sendHoverImagesPath;
    QString sendDisableImagesPath;
    bool isSending;
    // 已应用的屏 DPI（取整）：与目标 DPI 相同即跳过，避免重建样式表反复
    // 触发布局（DPI 缩放链与主窗口重排都会调用 applyDpiScale）
    int appliedDpi;
    // 已应用的字体像素字号：换屏自愈会先用旧字号按新 DPI 换算一次，随后
    // MainWindow 更新 AppContext 字号再调本方法，此时 DPI 未变但字号已变，
    // 只看 appliedDpi 会把字体（及占位符）停在旧字号上
    int appliedFontPixelSize;
};

template <typename T>
void TextEdit::connectSendButtonClick(T *receiver, void (T::*slot)())
{
    connect(sendButton, &QPushButton::clicked, [=](bool) { (receiver->*slot)(); });
}

template <typename T>
void TextEdit::connectSendButtonClick(T *receiver, void (T::*slot)(bool))
{
    connect(sendButton, &QPushButton::clicked, receiver, slot);
}

#endif // TEXTEDIT_H
