#include "mainwindow.h"

#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    qputenv("QT_QUICK_BACKEND", "software");
    qputenv("QTWEBENGINE_CHROMIUM_FLAGS",
            "--disable-gpu "
            "--disable-software-rasterizer "
            "--disable-gpu-compositing "
            "--disable-gpu-sandbox "
            "--no-sandbox "
            "--disable-dev-shm-usage "
            "--disable-features=VizDisplayCompositor");
    qputenv("QTWEBENGINE_DISABLE_SANDBOX", "1");
    QCoreApplication::setAttribute(Qt::AA_UseSoftwareOpenGL);

    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}
