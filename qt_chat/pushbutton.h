#ifndef PUSHBUTTON_H
#define PUSHBUTTON_H

#pragma once
#include <QPushButton>
#include <QMouseEvent>
#include <QEvent>
#include <QPoint>
#include <QFont>
#include <QFontDatabase>
#include <QToolTip>

extern const QString fontFilePath;
extern const int buttonFontPointSize;

class PushButton : public QPushButton
{
    Q_OBJECT
public:
    explicit PushButton(const QString &tipText = "",
                        int tipOffsetX = 10,
                        int tipOffsetY = 40,
                        QWidget *parent = nullptr);
    ~PushButton();

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    bool event(QEvent *event) override;

private:
    QString m_tipText;
    QPoint  m_tipStartPos;
    int     m_ox;
    int     m_oy;
};

#endif // PUSHBUTTON_H
