#ifndef COPYBUTTON_H
#define COPYBUTTON_H

#pragma once
#include <QtWidgets/QPushButton>
#include <QtGui/QFont>
#include <QtGui/QFontDatabase>
#include <QtGui/QClipboard>
#include <QtWidgets/QToolTip>
#include <QtCore/QEvent>
#include <QtCore/QPoint>
#include <QtWidgets/QApplication>

// extern const QString fontFilePath;
// extern const int buttonFontPointSize;

class MessageWidget;
class AppContext;

class CopyButton : public QPushButton
{
    Q_OBJECT
public:
    explicit CopyButton(AppContext *appContext, const QString &tipText = "", int tipOffsetX = 10,
                        int tipOffsetY = 40, QWidget *parent = nullptr);
    ~CopyButton();

protected:
    void mousePressEvent(QMouseEvent *e) override;
    void mouseReleaseEvent(QMouseEvent *e) override;
    bool event(QEvent *e) override;

private:
    MessageWidget *getMessageWidget();

    AppContext *appContext;
    QString tipText;
    QPoint tipStartPos;
};

#endif // COPYBUTTON_H
