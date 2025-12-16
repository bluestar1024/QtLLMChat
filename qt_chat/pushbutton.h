#ifndef PUSHBUTTON_H
#define PUSHBUTTON_H

#pragma once
#include <QtWidgets/QPushButton>
#include <QtGui/QMouseEvent>
#include <QtCore/QEvent>
#include <QtCore/QPoint>
#include <QtGui/QFont>
#include <QtGui/QFontDatabase>
#include <QtWidgets/QToolTip>

extern const QString fontFilePath;
extern const int buttonFontPointSize;

class PushButton : public QPushButton
{
    Q_OBJECT
public:
    explicit PushButton(const QString &tipText = "", int tipOffsetX = 10, int tipOffsetY = 40,
                        QWidget *parent = nullptr);
    ~PushButton();

protected:
    void mousePressEvent(QMouseEvent *e) override;
    void mouseReleaseEvent(QMouseEvent *e) override;
    bool event(QEvent *e) override;

private:
    QString tipText;
    QPoint tipStartPos;
    int tipOffsetX;
    int tipOffsetY;
};

#endif // PUSHBUTTON_H
