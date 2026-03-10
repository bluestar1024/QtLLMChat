#include "mainwindow.h"

#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    qputenv("QSG_RHI_BACKEND", "null");
    qputenv("QT_QUICK_BACKEND", "software");
    qputenv("QTWEBENGINE_CHROMIUM_FLAGS",
            "--disable-gpu "
            "--disable-gpu-compositing "
            "--disable-gpu-rasterization "
            "--disable-software-rasterizer "
            "--single-process "
            "--no-sandbox "
            "--disable-dev-shm-usage ");
    qputenv("QT_LOGGING_RULES", "qt.webenginecontext.debug=true");

    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}
