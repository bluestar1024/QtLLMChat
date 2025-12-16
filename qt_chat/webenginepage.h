#ifndef WEBENGINEPAGE_H
#define WEBENGINEPAGE_H

#pragma once
#include <QWebEnginePage>
#include <QMainWindow>
#include <QList>
#include "customwebengineview.h"

class WebEnginePage : public QWebEnginePage
{
    Q_OBJECT
public:
    explicit WebEnginePage(QObject *parent = nullptr);
    ~WebEnginePage();

protected:
    bool acceptNavigationRequest(const QUrl &url, NavigationType type, bool isMainFrame) override;

private:
    QList<QMainWindow *> windows;
};
#endif // WEBENGINEPAGE_H
