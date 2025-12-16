#include "customwebengineview.h"

CustomWebEngineView::CustomWebEngineView(QWidget *parent) : QWebEngineView(parent) { }

CustomWebEngineView::~CustomWebEngineView() { }

CustomWebEngineView *CustomWebEngineView::createWindow(QWebEnginePage::WebWindowType type)
{
    Q_UNUSED(type)
    auto *newView = new CustomWebEngineView();
    auto *newWindow = new QMainWindow();
    newWindow->setCentralWidget(newView);
    newWindow->resize(1200, 800);
    newWindow->show();
    windows.append(newWindow);
    connect(newWindow, &QMainWindow::destroyed, this, [this](QObject *obj) {
        QMainWindow *window = qobject_cast<QMainWindow *>(obj);
        if (window)
            windows.removeOne(window);
    });
    return newView;
}
