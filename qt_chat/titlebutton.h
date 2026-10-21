#ifndef TITLEBUTTON_H
#define TITLEBUTTON_H

#pragma once
#include <QtWidgets/QPushButton>
#include <QtGui/QMouseEvent>
#include <QtCore/QEvent>
#include <QtWidgets/QToolTip>

class AppContext;

class TitleButton : public QPushButton
{
    Q_OBJECT
public:
    explicit TitleButton(AppContext *appContext, const QString &tipText = "",
                         QWidget *parent = nullptr);
    ~TitleButton();

    void setTipText(const QString &tipText);

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    bool event(QEvent *event) override;

private:
    AppContext *appContext;
    QString tipText;
};

#endif // TITLEBUTTON_H
