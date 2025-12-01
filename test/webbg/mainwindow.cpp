#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    m_webEngineView = new QWebEngineView();
    m_webEngineView->setUrl(QUrl("https://www.baidu.com"));
    m_webEngineView->show();
    //    m_webEngineView->setFixedSize(400, 200);
    setCentralWidget(m_webEngineView);
    resize(m_webEngineView->size());
}

MainWindow::~MainWindow() {}
