#ifndef WEBENGINEPAGE_H
#define WEBENGINEPAGE_H

#pragma once
#include "customwebengineview.h"

#include <QtWebEngineCore/QWebEnginePage>
#include <QtWidgets/QMainWindow>
#include <QtCore/QUrl>
#include <QtCore/QList>

class WebEnginePage : public QWebEnginePage
{
    Q_OBJECT
public:
    explicit WebEnginePage(QWebEngineProfile *profile, QObject *parent = nullptr);
    ~WebEnginePage();

protected:
    bool acceptNavigationRequest(const QUrl &url, NavigationType type, bool isMainFrame) override;

private:
    QList<QMainWindow *> windows;
};
#endif // WEBENGINEPAGE_H
