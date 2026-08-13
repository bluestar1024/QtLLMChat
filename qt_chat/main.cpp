#include "mainwindow.h"

#include <QtWidgets/QApplication>
#include <Qt3DCore/QAttribute>
#include <QtGui/QOpenGLContext>
#include <QDebug>

#include <windows.h>
#include <cstdio>
#include <cinttypes>

// ---- 崩溃捕获（调试用）：记录异常代码与调用栈到 crash.log ----
// 纯 Win32 实现，崩溃时即使堆已损坏也只尝试一次写入，失败则直接退出
static void dumpModuleAndOffset(FILE *f, const void *addr)
{
    HMODULE h = nullptr;
    if (GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS
                                   | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                           static_cast<LPCWSTR>(addr), &h)) {
        wchar_t path[MAX_PATH];
        DWORD n = GetModuleFileNameW(h, path, MAX_PATH);
        const wchar_t *base = path;
        for (DWORD i = 0; i < n; ++i) {
            if (path[i] == L'\\' || path[i] == L'/')
                base = path + i + 1;
        }
        fprintf(f, "%ls+0x%" PRIXPTR, base,
                reinterpret_cast<uintptr_t>(addr) - reinterpret_cast<uintptr_t>(h));
    } else {
        fprintf(f, "%p(no-module)", addr);
    }
}

static LONG NTAPI CrashHandler(PEXCEPTION_POINTERS ep)
{
    FILE *f = nullptr;
    if (fopen_s(&f, "crash.log", "w") == 0 && f) {
        fprintf(f, "ExceptionCode: 0x%08lX\n", ep->ExceptionRecord->ExceptionCode);
        fprintf(f, "CrashAddress: ");
        dumpModuleAndOffset(f, ep->ExceptionRecord->ExceptionAddress);
        fprintf(f, "\n");
        fprintf(f, "ContextRip: 0x%llX  ContextRsp: 0x%llX  ContextRbp: 0x%llX\n",
                static_cast<unsigned long long>(ep->ContextRecord->Rip),
                static_cast<unsigned long long>(ep->ContextRecord->Rsp),
                static_cast<unsigned long long>(ep->ContextRecord->Rbp));

        // 从崩溃 CONTEXT 手动解栈（RtlVirtualUnwind），穿过异常分发器拿到完整调用链
        typedef PRUNTIME_FUNCTION(NTAPI *LookupFn)(DWORD64, PDWORD64, PUNWIND_HISTORY_TABLE);
        typedef NTSTATUS(NTAPI *UnwindFn)(ULONG, DWORD64, DWORD64, PRUNTIME_FUNCTION, PCONTEXT,
                                          PVOID *, PDWORD64, PKNONVOLATILE_CONTEXT_POINTERS);
        HMODULE ntdll = GetModuleHandleW(L"ntdll.dll");
        auto pLookup = reinterpret_cast<LookupFn>(GetProcAddress(ntdll, "RtlLookupFunctionEntry"));
        auto pUnwind = reinterpret_cast<UnwindFn>(GetProcAddress(ntdll, "RtlVirtualUnwind"));

        fprintf(f, "FullStack:\n");
        if (pLookup && pUnwind) {
            CONTEXT c = *ep->ContextRecord;
            for (int i = 0; i < 64; ++i) {
                fprintf(f, "  #%d ", i);
                dumpModuleAndOffset(f, reinterpret_cast<void *>(c.Rip));
                fprintf(f, "\n");
                DWORD64 imageBase = 0;
                PRUNTIME_FUNCTION fn = pLookup(c.Rip, &imageBase, nullptr);
                if (!fn) {
                    fprintf(f, "  [lookup failed at #%d]\n", i + 1);
                    break;
                }
                void *handlerData = nullptr;
                DWORD64 establisher = 0;
                KNONVOLATILE_CONTEXT_POINTERS ctxPtrs;
                memset(&ctxPtrs, 0, sizeof(ctxPtrs));
                NTSTATUS st = pUnwind(UNW_FLAG_NHANDLER, imageBase, c.Rip, fn, &c, &handlerData,
                                      &establisher, &ctxPtrs);
                if (st != 0) {
                    fprintf(f, "  [unwind failed: 0x%lX at #%d]\n",
                            static_cast<unsigned long>(st), i + 1);
                    break;
                }
            }
        } else {
            const int kMaxFrames = 64;
            void *frames[kMaxFrames] = { nullptr };
            USHORT count = CaptureStackBackTrace(0, kMaxFrames, frames, nullptr);
            for (USHORT i = 0; i < count; ++i) {
                fprintf(f, "  #%u ", i);
                dumpModuleAndOffset(f, frames[i]);
                fprintf(f, "\n");
            }
        }

        // 栈扫描兜底：不依赖 unwind 信息，从 RSP 向上逐 8 字节扫描栈内存，
        // 凡落在已加载模块范围内的值即为可能的返回地址
        fprintf(f, "StackScan (RSP=0x%llX):\n",
                static_cast<unsigned long long>(ep->ContextRecord->Rsp));
        {
            const DWORD64 rsp = ep->ContextRecord->Rsp;
            for (DWORD64 p = rsp & ~7ULL; p < rsp + 0x10000; p += 8) {
                DWORD64 val = 0;
                SIZE_T read = 0;
                if (!ReadProcessMemory(GetCurrentProcess(), reinterpret_cast<LPCVOID>(p), &val,
                                       sizeof(val), &read)
                    || read != sizeof(val)) {
                    break;
                }
                HMODULE h = nullptr;
                if (GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS
                                               | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                                       reinterpret_cast<LPCWSTR>(val), &h)) {
                    fprintf(f, "  [+%08llX] ", static_cast<unsigned long long>(p - rsp));
                    dumpModuleAndOffset(f, reinterpret_cast<void *>(val));
                    fprintf(f, "\n");
                }
            }
        }

        // RBP 链（Release 优化下可能无帧指针，作为补充）
        fprintf(f, "RbpChain:\n");
        {
            DWORD64 rbp = ep->ContextRecord->Rbp;
            for (int i = 0; i < 32 && rbp != 0; ++i) {
                DWORD64 next = 0, ret = 0;
                SIZE_T r1 = 0, r2 = 0;
                ReadProcessMemory(GetCurrentProcess(), reinterpret_cast<LPCVOID>(rbp), &next,
                                  sizeof(next), &r1);
                ReadProcessMemory(GetCurrentProcess(),
                                  reinterpret_cast<LPCVOID>(rbp + 8), &ret, sizeof(ret), &r2);
                if (r1 != sizeof(next) || r2 != sizeof(ret))
                    break;
                fprintf(f, "  RBP#%d ", i);
                dumpModuleAndOffset(f, reinterpret_cast<void *>(ret));
                fprintf(f, "\n");
                rbp = next;
            }
        }
        fclose(f);
    }
    // 已记录崩溃现场，直接终止进程（与原有闪退行为一致，不再弹 Windows 错误框）
    return EXCEPTION_EXECUTE_HANDLER;
}

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
    // 注册崩溃处理器：任何未处理异常（含访问违规 0xc0000005）都会写入 crash.log
    // AddVectoredExceptionHandler(1, CrashHandler);

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
