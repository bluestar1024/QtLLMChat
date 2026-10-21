#ifndef SENDBUTTON_H
#define SENDBUTTON_H

#pragma once
#include <QtWidgets/QPushButton>
#include <QtCore/QEvent>
#include <QtWidgets/QToolTip>

// extern const QString fontFilePath;
// extern const int buttonFontPointSize;

class AppContext;

class SendButton : public QPushButton
{
    Q_OBJECT
public:
    explicit SendButton(AppContext *appContext, const QString &tipText = "",
                        QWidget *parent = nullptr);
    ~SendButton();

protected:
    bool event(QEvent *e) override;

private:
    AppContext *appContext;
    QString tipText;
};

#endif // SENDBUTTON_H
