#include "webenginepage.h"

WebEnginePage::WebEnginePage(QObject *parent)
    : QWebEnginePage(parent)
{
}

WebEnginePage::~WebEnginePage()
{
}

bool WebEnginePage::acceptNavigationRequest(const QUrl &url, NavigationType type, bool isMainFrame)
{
    if (type == QWebEnginePage::NavigationTypeLinkClicked) {
        auto *newView = new CustomWebEngineView();
        newView->setUrl(url);
        auto *newWindow = new QMainWindow();
        newWindow->setCentralWidget(newView);
        newWindow->resize(1200, 800);
        newWindow->show();
        windows.append(newWindow);
        // 移除窗口时清理指针
        connect(newWindow, &QMainWindow::destroyed,
                this, [this](QObject *obj) {
                        QMainWindow *window = qobject_cast<QMainWindow*>(obj);
                        if (window) windows.removeOne(window);
                        }
        );
        return false;  // 阻止当前视图跳转
    }
    return QWebEnginePage::acceptNavigationRequest(url, type, isMainFrame);
//    return true;
}
