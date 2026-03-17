#include "mainwindow.h"

#include <QtWidgets/QApplication>
#include <Qt3DCore/QAttribute>
#include <QDebug>

void myMessageHandler(QtMsgType type, const QMessageLogContext &context,
                      const QString &msg)
{
    // 捕获 Qt 警告和调试信息
    if (type == QtWarningMsg || type == QtCriticalMsg) {
        qDebug() << "捕获到:" << msg;
        qDebug() << "位置:" << context.file << context.line;
    }
}

int main(int argc, char *argv[])
{
    // qputenv("QSG_RHI_BACKEND", "null");
    // qputenv("QT_QUICK_BACKEND", "software");
    // qputenv("QTWEBENGINE_CHROMIUM_FLAGS",
    //         "--disable-gpu "
    //         "--disable-gpu-compositing "
    //         "--disable-gpu-rasterization "
    //         "--disable-software-rasterizer "
    //         "--single-process "
    //         "--no-sandbox "
    //         "--disable-dev-shm-usage ");
    // qputenv("QT_LOGGING_RULES", "qt.webenginecontext.debug=true");

    // qputenv("QT_OPENGL", "desktop");
    // qputenv("QT_ANGLE_PLATFORM", "opengl");
    qputenv("QT_ANGLE_PLATFORM", "d3d11");
    // 在 main 中设置
    qInstallMessageHandler(myMessageHandler);
    Qt3DCore::QAttribute *texCoordAttribute = new Qt3DCore::QAttribute();
    texCoordAttribute->setVertexBaseType(Qt3DCore::QAttribute::Short);
    texCoordAttribute->setVertexSize(2);
    texCoordAttribute->setName(Qt3DCore::QAttribute::defaultTextureCoordinate2AttributeName());

    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}
