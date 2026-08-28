#ifndef TITLEBUTTON_H
#define TITLEBUTTON_H

#pragma once
#include <QtWidgets/QPushButton>
#include <QtGui/QMouseEvent>
#include <QtCore/QEvent>
#include <QtWidgets/QToolTip>
#include <QtGui/QFontDatabase>
#include <QtGui/QFont>
#include <QtCore/QPoint>

class AppContext;

class TitleButton : public QPushButton
{
    Q_OBJECT
public:
    explicit TitleButton(AppContext *appContext, const QString &tipText = "", int tipOffsetX = 10,
                         int tipOffsetY = 40, QWidget *parent = nullptr);
    ~TitleButton();

    void setTipText(const QString &tipText);
    void setTipOffset(int x, int y);

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    bool event(QEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;

private:
    void updateTipStartPos();

    AppContext *appContext;
    QString tipText;
    int tipOffsetX;
    int tipOffsetY;
    QPoint tipStartPos;
};

#endif // TITLEBUTTON_H
