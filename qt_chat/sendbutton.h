#ifndef SENDBUTTON_H
#define SENDBUTTON_H

#pragma once
#include <QtWidgets/QPushButton>
#include <QtCore/QEvent>
#include <QtGui/QFont>
#include <QtGui/QFontDatabase>
#include <QtWidgets/QToolTip>
#include <QtCore/QPoint>

extern const QString fontFilePath;
extern const int buttonFontPointSize;

class SendButton : public QPushButton
{
    Q_OBJECT
public:
    explicit SendButton(const QString &tipText = "", int tipOffsetX = 10, int tipOffsetY = 40,
                        QWidget *parent = nullptr);
    ~SendButton();

protected:
    bool event(QEvent *e) override;

private:
    QString tipText;
    QPoint tipStartPos;
};

#endif // SENDBUTTON_H
