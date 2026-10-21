#ifndef PUSHBUTTON_H
#define PUSHBUTTON_H

#pragma once
#include <QtWidgets/QPushButton>
#include <QtGui/QMouseEvent>
#include <QtCore/QEvent>
#include <QtWidgets/QToolTip>

// extern const QString fontFilePath;
// extern const int buttonFontPointSize;

class AppContext;

class PushButton : public QPushButton
{
    Q_OBJECT
public:
    explicit PushButton(AppContext *appContext, const QString &tipText = "",
                        QWidget *parent = nullptr);
    ~PushButton();

    void setTipText(QString tipText);
    QString getTipText();

protected:
    void mousePressEvent(QMouseEvent *e) override;
    void mouseReleaseEvent(QMouseEvent *e) override;
    bool event(QEvent *e) override;

private:
    AppContext *appContext;
    QString tipText;
};

#endif // PUSHBUTTON_H
