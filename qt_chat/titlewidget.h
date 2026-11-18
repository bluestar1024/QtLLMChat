#ifndef TITLEWIDGET_H
#define TITLEWIDGET_H

#pragma once
#include "widget.h"
#include "titlebutton.h"

#include <QtWidgets/QWidget>
#include <QtGui/QPainter>
#include <QtGui/QPainterPath>
#include <QtGui/QBrush>
#include <QtGui/QColor>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QSizePolicy>
#include <QtGui/QIcon>
#include <QtGui/QPixmap>
#include <QtCore/QSize>

class AppContext;

class TitleWidget : public QWidget
{
    Q_OBJECT
public:
    explicit TitleWidget(AppContext *appContext, QWidget *parent = nullptr);
    ~TitleWidget();

    // void setRoundAngle();
    // void setRightAngle();
    void maxButtonToggleIcon(bool isMaxIcon = true);
    QSize getMinButtonSize();
    QSize getMaxButtonSize();
    QSize getCloseButtonSize();
    void saveWidgetSize();
    // 按 144 DPI 基准（DpiUtil::kBaseDpi）重设各按钮/图标与标题栏高度
    void updateSize(qreal curDpi, qreal lastDpi);

signals:
    void minimizeClicked();
    void maximizeClicked();
    void closeClicked();

protected:
    void paintEvent(QPaintEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    // void mousePressEvent(QMouseEvent *e) override;
    // void mouseReleaseEvent(QMouseEvent *e) override;

private:
    void initializeUI();
    void setupLeftWidget();
    void setupRightWidget();
    void setupMainLayout();

    AppContext *appContext;
    QLabel *titleIconLabel;
    TitleButton *minButton;
    TitleButton *maxButton;
    TitleButton *closeButton;
    Widget *titleLeftSubWidget;
    Widget *titleRightSubWidget;

    // bool isRound;
    QString maxImagePath;
    QString normalImagePath;
    // 各尺寸缓存（widgetSizeDict）以 144 DPI 基准保存：保存时按当前已应用的
    // DPI 折算回基准，读取时再按目标 DPI 放大，来回跨屏不会累积取整误差
    QMap<QString, QVariant> widgetSizeDict;
    qreal appliedDpi;
};

#endif // TITLEWIDGET_H
