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

class TitleWidget : public QWidget
{
    Q_OBJECT
public:
    explicit TitleWidget(QWidget *parent = nullptr);
    ~TitleWidget();

    void setRoundAngle();
    void setRightAngle();
    void maxButtonToggleIcon(bool isMaxIcon = true);
    QSize getMinButtonSize();
    QSize getMaxButtonSize();
    QSize getCloseButtonSize();
    void updateSize(qreal curDpi, qreal lastDpi);

signals:
    void minimizeClicked();
    void maximizeClicked();
    void closeClicked();

protected:
    void paintEvent(QPaintEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;

private:
    void initializeUI();
    void setupLeftWidget();
    void setupRightWidget();
    void setupMainLayout();

    QLabel *titleIconLabel;
    TitleButton *minButton;
    TitleButton *maxButton;
    TitleButton *closeButton;
    Widget *titleLeftSubWidget;
    Widget *titleRightSubWidget;

    bool isRound;
    QString maxImagePath;
    QString normalImagePath;
    QMap<QString, QVariant> widgetSizeDict;
};

#endif // TITLEWIDGET_H
