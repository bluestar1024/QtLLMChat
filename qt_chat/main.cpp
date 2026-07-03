#include "mainwindow.h"

#include <QtWidgets/QApplication>
#include <Qt3DCore/QAttribute>
#include <QtGui/QOpenGLContext>
#include <QDebug>

void myMessageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    // 捕获 Qt 警告和调试信息
    if (type == QtWarningMsg || type == QtCriticalMsg) {
        qDebug() << "捕获到:" << msg;
        qDebug() << "位置:" << context.file << context.line;
    }
}

void diagnoseGraphics()
{
    qDebug() << "\n===== 图形系统诊断 =====";

    // 屏幕信息
    QScreen *screen = QApplication::primaryScreen();
    qDebug() << "主屏幕:" << screen->name();
    qDebug() << "DPI:" << screen->logicalDotsPerInch();
    qDebug() << "物理尺寸:" << screen->physicalSize();

    // OpenGL 信息
    qDebug() << "OpenGL 类型:" << QOpenGLContext::openGLModuleType();
    QOpenGLContext ctx;
    if (ctx.create()) {
        qDebug() << "OpenGL 版本:" << ctx.format().version();
        qDebug() << "OpenGL 渲染器:" << ctx.format().renderableType();
    }

    // RHI 后端信息
    qDebug() << "RHI 后端:" << qgetenv("QSG_RHI_BACKEND");
    qDebug() << "ANGLE 后端:" << qgetenv("QT_ANGLE_PLATFORM");

    qDebug() << "======================\n";
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
    // QCoreApplication::setAttribute(Qt::AA_ShareOpenGLContexts);
    // QCoreApplication::setAttribute(Qt::AA_UseOpenGLES);
    // qputenv("QT_ANGLE_PLATFORM", "d3d11");
    // qputenv("QTWEBENGINE_CHROMIUM_FLAGS",
    //         "--ignore-gpu-blacklist "  // 忽略GPU黑名单
    //         "--enable-gpu-rasterization " // 启用GPU光栅化
    //         "--enable-native-gpu-memory-buffers"); // 启用原生GPU内存缓冲区
    // 在 main 中设置
    // qInstallMessageHandler(myMessageHandler);
    // Qt3DCore::QAttribute *texCoordAttribute = new Qt3DCore::QAttribute();
    // texCoordAttribute->setVertexBaseType(Qt3DCore::QAttribute::Short);
    // texCoordAttribute->setVertexSize(2);
    // texCoordAttribute->setName(Qt3DCore::QAttribute::defaultTextureCoordinate2AttributeName());

    qDebug() << "RHI 后端:" << qgetenv("QSG_RHI_BACKEND");
    qDebug() << "ANGLE 后端:" << qgetenv("QT_ANGLE_PLATFORM");

    // qputenv("QSG_RHI_BACKEND", "d3d11");
    // qputenv("QTWEBENGINE_CHROMIUM_FLAGS",
    //         "--ignore-gpu-blacklist "
    //         "--enable-gpu-rasterization "
    //         "--enable-native-gpu-memory-buffers");

    // qputenv("QT_SCALE_FACTOR", "0.66");
    // qputenv("QT_AUTO_SCREEN_SCALE_FACTOR", "1");
    // QCoreApplication::setAttribute(Qt::AA_DisableHighDpiScaling, false);
    // QApplication::setAttribute(Qt::AA_DisableHighDpiScaling);
    // QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    // QApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);
    // QGuiApplication::setHighDpiScaleFactorRoundingPolicy(
    //         Qt::HighDpiScaleFactorRoundingPolicy::PassThrough);

    qputenv("QT_ENABLE_HIGHDPI_SCALING", "0");
    // qputenv("QT_AUTO_SCREEN_SCALE_FACTOR", "0");

    // qputenv("QTWEBENGINE_CHROMIUM_FLAGS",
    //         "--disable-site-isolation-trials "
    //         "--disable-features=site-per-process "
    //         "--single-process");

    QApplication a(argc, argv);

    diagnoseGraphics();
    // a.setHighDpiScaleFactorRoundingPolicy(Qt::HighDpiScaleFactorRoundingPolicy::PassThrough);
    qDebug() << "DPI Rounding Policy:" << QGuiApplication::highDpiScaleFactorRoundingPolicy();
    QScreen *primary = QGuiApplication::primaryScreen();
    qDebug() << "设备像素比 (DPR):" << primary->devicePixelRatio();
    qreal systemDpr = primary->devicePixelRatio();
    QString scaleFactor = QString::number(systemDpr, 'f', 2);
    qputenv("QT_SCALE_FACTOR", scaleFactor.toLatin1());
    qDebug() << "=== 系统DPI缩放信息 ===";
    qDebug() << "屏幕名称:" << primary->name();
    qDebug() << "设备像素比 (DPR):" << primary->devicePixelRatio();
    qDebug() << "逻辑DPI:" << primary->logicalDotsPerInch();
    qDebug() << "物理DPI:" << primary->physicalDotsPerInch();
    qDebug() << "原生分辨率:" << primary->geometry().size() * primary->devicePixelRatio();

    MainWindow w;
    w.show();
    return a.exec();
}
