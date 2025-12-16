#ifndef SENDBUTTON_H
#define SENDBUTTON_H

#pragma once
#include <QPushButton>
#include <QEvent>
#include <QFont>
#include <QFontDatabase>
#include <QToolTip>
#include <QPoint>

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
    bool event(QEvent *event) override;

private:
    QString m_tipText;
    QPoint m_tipStartPos;
    int m_ox;
    int m_oy;
};

#endif // SENDBUTTON_H
