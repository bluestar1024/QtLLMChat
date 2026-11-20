#ifndef FUNWIDGET_H
#define FUNWIDGET_H

#pragma once
#include "pushbutton.h"
#include "widget.h"

#include <QtWidgets/QWidget>
#include <QtWidgets/QLabel>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QSizePolicy>
#include <QtGui/QFont>
#include <QtGui/QIcon>
#include <QtGui/QMouseEvent>
#include <QtCore/QMap>
#include <QtCore/QString>
#include <QtCore/QMargins>
#include <QtCore/QSize>
#include <QtCore/QDir>
#include <QtCore/QVariant>

// extern const QString imagesDir;
// extern const QString fontFilePath;
// extern const int titleFontPixelSize;

class AppContext;

class FunWidget : public QWidget
{
    Q_OBJECT
public:
    explicit FunWidget(AppContext *appContext, QWidget *parent = nullptr);
    ~FunWidget();

    template <typename T>
    void connectChatRecordsButtonClick(T *receiver, void (T::*slot)());
    template <typename T>
    void connectChatRecordsButtonClick(T *receiver, void (T::*slot)(bool));
    template <typename T>
    void connectNewChatButtonClick(T *receiver, void (T::*slot)());
    template <typename T>
    void connectNewChatButtonClick(T *receiver, void (T::*slot)(bool));

    void setSize();
    void saveWidgetSize();
    void resetWidgetSize();
    // 按 144 DPI 基准（DpiUtil::kBaseDpi）重设功能区尺寸与内部边距
    void updateSize(qreal curDpi, qreal lastDpi);

protected:
    void mouseMoveEvent(QMouseEvent *e) override;

private:
    AppContext *appContext;
    PushButton *chatRecordsButton;
    PushButton *newChatButton;
    QString chatRecordsImagesPath;
    QString newChatImagesPath;
    QLabel *titleLabel;
    QFont titleFont;

    Widget *funLeftSubWidget;
    Widget *funMidSubWidget;
    Widget *funRightSubWidget;

    QHBoxLayout *funLeftSubHLayout;
    QHBoxLayout *funMidSubHLayout;
    QHBoxLayout *funRightSubHLayout;
    QHBoxLayout *mainHLayout;

    // 各尺寸缓存（widgetSizeDict）以 144 DPI 基准保存：保存时按当前已应用的
    // DPI 折算回基准，读取时再按目标 DPI 放大，来回跨屏不会累积取整误差
    QMap<QString, QVariant> widgetSizeDict;
    qreal appliedDpi;

    /* 记录原始尺寸 */
    // struct Saved
    // {
    //     QSize self;
    //     QSize chatBtn;
    //     QSize chatIcon;
    //     QSize leftW;
    //     QSize midW;
    //     QSize rightW;
    //     QSize title;
    //     QSize newBtn;
    //     QSize newIcon;
    //     QMargins leftMargin;
    //     QMargins midMargin;
    //     QMargins rightMargin;
    //     QMargins mainMargin;
    // };
    // Saved saved;
};

Q_DECLARE_METATYPE(QMargins)

template <typename T>
void FunWidget::connectChatRecordsButtonClick(T *receiver, void (T::*slot)())
{
    connect(chatRecordsButton, &QPushButton::clicked, [=](bool) { (receiver->*slot)(); });
}

template <typename T>
void FunWidget::connectChatRecordsButtonClick(T *receiver, void (T::*slot)(bool))
{
    connect(chatRecordsButton, &QPushButton::clicked, receiver, slot);
}

template <typename T>
void FunWidget::connectNewChatButtonClick(T *receiver, void (T::*slot)())
{
    connect(newChatButton, &QPushButton::clicked, [=](bool) { (receiver->*slot)(); });
}

template <typename T>
void FunWidget::connectNewChatButtonClick(T *receiver, void (T::*slot)(bool))
{
    connect(newChatButton, &QPushButton::clicked, receiver, slot);
}

#endif // FUNWIDGET_H
