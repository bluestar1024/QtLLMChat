#ifndef TITLEWIDGET_H
#define TITLEWIDGET_H

#pragma once
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

class QLabel;
class TitleButton;

class TitleWidget : public QWidget
{
    Q_OBJECT

public:
    explicit TitleWidget(QWidget *parent = nullptr);

    void setRoundAngle();
    void setRightAngle();

    TitleButton *minimizeButton() const;
    TitleButton *maximizeButton() const;
    TitleButton *closeButton() const;

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

    bool isRound;

    QLabel *titleIconLabel;
    QWidget *titleLeftSubWidget;
    QWidget *titleRightSubWidget;
    TitleButton *minButton;
    TitleButton *maxButton;
    TitleButton *closeButton;

    QString maxPath;
    QString normalPath;
};

#endif // TITLEWIDGET_H
