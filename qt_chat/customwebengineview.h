#ifndef CUSTOMWEBENGINEVIEW_H
#define CUSTOMWEBENGINEVIEW_H

#pragma once
#include <QtWebEngineWidgets/QWebEngineView>
#include <QtWebEngineCore/QWebEnginePage>
#include <QtWidgets/QMainWindow>

class CustomWebEngineView : public QWebEngineView
{
    Q_OBJECT
public:
    explicit CustomWebEngineView(QWidget *parent = nullptr);
    ~CustomWebEngineView();

protected:
    CustomWebEngineView *createWindow(QWebEnginePage::WebWindowType) override;

private:
    QList<QMainWindow *> windows;
};

#endif // CUSTOMWEBENGINEVIEW_H
