#ifndef WEBENGINEVIEW_H
#define WEBENGINEVIEW_H

#pragma once
#include "listwidget.h"
#include "webenginepage.h"

#include <QtWebEngineWidgets/QWebEngineView>
#include <QtGui/QMouseEvent>
#include <QtGui/QWheelEvent>
#include <QtGui/QContextMenuEvent>
#include <QtCore/QEvent>
#include <QtCore/QCoreApplication>
#include <QtWidgets/QScrollBar>
#include <QtGui/QPointingDevice>

class TextShow;
class MessageWidget;

class WebEngineView : public QWebEngineView
{
    Q_OBJECT
public:
    explicit WebEngineView(QWidget *parent = nullptr);
    ~WebEngineView();

    template <typename T>
    void connectLoadFinished(T *receiver, void (T::*slot)(bool));
    template <typename T>
    void connectContentsSizeChanged(T *receiver, void (T::*slot)());
    template <typename T>
    void connectContentsSizeChanged(T *receiver, void (T::*slot)(QSize));

protected:
    bool eventFilter(QObject *o, QEvent *e) override;
    void contextMenuEvent(QContextMenuEvent *e) override;
    void wheelEvent(QWheelEvent *e) override;

private:
    ListWidget *findListWidget();
};

template <typename T>
void WebEngineView::connectLoadFinished(T *receiver, void (T::*slot)(bool))
{
    connect(this, &QWebEngineView::loadFinished, receiver, slot);
}

template <typename T>
void WebEngineView::connectContentsSizeChanged(T *receiver, void (T::*slot)())
{
    connect(page(), &QWebEnginePage::contentsSizeChanged, [=](QSize) { (receiver->*slot)(); });
}

template <typename T>
void WebEngineView::connectContentsSizeChanged(T *receiver, void (T::*slot)(QSize))
{
    connect(page(), &QWebEnginePage::contentsSizeChanged, receiver, slot);
}

#endif // WEBENGINEVIEW_H
