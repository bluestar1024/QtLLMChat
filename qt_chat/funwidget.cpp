#include "funwidget.h"
#include "appcontext.h"
#include "dpiutil.h"

#include <QDebug>

FunWidget::FunWidget(AppContext *appContext, QWidget *parent)
    : QWidget(parent), appContext(appContext), appliedDpi(DpiUtil::kBaseDpi)
{
    chatRecordsButton = new PushButton(this->appContext, "聊天历史");
    chatRecordsButton->setFixedSize(44, 44);
    chatRecordsImagesPath = QDir(this->appContext->imagesDir()).filePath("chat_records.png");
    chatRecordsButton->setIcon(QIcon(chatRecordsImagesPath));
    chatRecordsButton->setIconSize(QSize(30, 30));
    chatRecordsButton->setStyleSheet("QPushButton{ border:none; border-radius:22px; }"
                                     "QPushButton:hover{ background:#d0d0d0; }");
    funLeftSubWidget = new Widget();
    funLeftSubWidget->resize(chatRecordsButton->width() + 15, chatRecordsButton->height() + 16);
    funLeftSubWidget->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    // funLeftSubWidget->setObjectName("funLeftSubWidget");
    // funLeftSubWidget->setStyleSheet("QWidget {"
    //                                 "    background: green;"
    //                                 "}");
    funLeftSubHLayout = new QHBoxLayout(funLeftSubWidget);
    funLeftSubHLayout->addWidget(chatRecordsButton);
    funLeftSubHLayout->setAlignment(Qt::AlignLeft);
    funLeftSubHLayout->setContentsMargins(10, 10, 5, 6);
    titleLabel = new QLabel();
    titleLabel->setFixedHeight(60);
    // 字体族由 AppContext 统一注册缓存（原先自行 addApplicationFont 且
    // applicationFontFamilies(fontId).at(0) 未判空，空列表时越界读取崩溃）
    const QString &fontFamily = this->appContext->fontFamily();
    if (!fontFamily.isEmpty()) {
        titleFont = QFont(fontFamily);
        titleFont.setPixelSize(this->appContext->titleFontPixelSize());
        titleLabel->setFont(titleFont);
    }
    titleLabel->setText("AI助理");
    titleLabel->adjustSize();
    titleLabel->setFixedWidth(titleLabel->width() + 2);
    funMidSubWidget = new Widget();
    funMidSubWidget->resize(titleLabel->size());
    funMidSubWidget->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    // funMidSubWidget->setObjectName("funMidSubWidget");
    // funMidSubWidget->setStyleSheet("QWidget {"
    //                                "    background: green;"
    //                                "}");
    funMidSubHLayout = new QHBoxLayout(funMidSubWidget);
    funMidSubHLayout->addWidget(titleLabel);
    funMidSubHLayout->setAlignment(Qt::AlignCenter);
    funMidSubHLayout->setContentsMargins(0, 0, 0, 0);
    newChatButton = new PushButton(this->appContext, "新聊天");
    newChatButton->setFixedSize(44, 44);
    newChatImagesPath = QDir(this->appContext->imagesDir()).filePath("new_chat.png");
    newChatButton->setIcon(QIcon(newChatImagesPath));
    newChatButton->setIconSize(QSize(30, 30));
    newChatButton->setStyleSheet("QPushButton{ border:none; border-radius:22px; }"
                                 "QPushButton:hover{ background:#d0d0d0; }");
    funRightSubWidget = new Widget();
    funRightSubWidget->resize(newChatButton->width() + 15, newChatButton->height() + 16);
    funRightSubWidget->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    // funRightSubWidget->setObjectName("funRightSubWidget");
    // funRightSubWidget->setStyleSheet("QWidget {"
    //                                  "    background: green;"
    //                                  "}");
    funRightSubHLayout = new QHBoxLayout(funRightSubWidget);
    funRightSubHLayout->addWidget(newChatButton);
    funRightSubHLayout->setAlignment(Qt::AlignRight);
    funRightSubHLayout->setContentsMargins(5, 10, 10, 6);
    mainHLayout = new QHBoxLayout(this);
    mainHLayout->addWidget(funLeftSubWidget);
    mainHLayout->addWidget(funMidSubWidget);
    mainHLayout->addWidget(funRightSubWidget);
    mainHLayout->setContentsMargins(0, 0, 0, 0);

    setFixedSize(1200, 60);
    // setObjectName("this");
    // setStyleSheet("QWidget {"
    //               "    background: blue;"
    //               "}");
    // qDebug() << "funRightSubHLayout contentsmargins:" << funRightSubHLayout->contentsMargins();
    // qDebug() << "funLeftSubWidget size:" << funLeftSubWidget->size();
    // qDebug() << "funMidSubWidget size:" << funMidSubWidget->size();
    // qDebug() << "funRightSubWidget size:" << funRightSubWidget->size();
    setMouseTracking(true);
    saveWidgetSize();
}

FunWidget::~FunWidget() { }

void FunWidget::mouseMoveEvent(QMouseEvent *e)
{
    QWidget::mouseMoveEvent(e);
    e->ignore();
}

void FunWidget::setSize()
{
    // qDebug() << "FunWidget size:" << size();
    // qDebug() << "funLeftSubWidget geometry:" << funLeftSubWidget->geometry();
    // qDebug() << "funMidSubWidget geometry:" << funMidSubWidget->geometry();
    // qDebug() << "funRightSubWidget geometry:" << funRightSubWidget->geometry();
    // qDebug() << "funLeftSubHLayout contentsmargins:" << funLeftSubHLayout->contentsMargins();
    // qDebug() << "funMidSubHLayout contentsmargins:" << funMidSubHLayout->contentsMargins();
    // qDebug() << "funRightSubHLayout contentsmargins:" << funRightSubHLayout->contentsMargins();
    // qDebug() << "chatRecordsButton geometry:" << chatRecordsButton->geometry();
    // qDebug() << "titleLabel geometry:" << titleLabel->geometry();
    // qDebug() << "newChatButton geometry:" << newChatButton->geometry();

    // 三个子部件的几何由 mainHLayout 计算：三者水平尺寸策略均为 Preferred 且未设置
    // 拉伸因子，QHBoxLayout 按相等宽度分配——左右两侧等宽保证中间标题水平居中，
    // 右侧子部件右边缘与 FunWidget 右边缘重合，其内部布局右边距（10px）就是
    // newChatButton 与 FunWidget 右边缘之间的间隙。
    // 不能在这里手工 resize 左右子部件：(width() - funMidSubWidget->width()) / 2
    // 没有计入布局间距（144 DPI 下为 9px），且 resize() 只改尺寸不改位置，右侧
    // 子部件会整体右移、右边缘超出 FunWidget 一个间距，内部 10px 右边距被裁掉，
    // newChatButton 便贴到 FunWidget 右边缘；动画最后一帧 FunWidget 尺寸不再变化，
    // 不会再次触发布局计算，该错误几何会一直保留。
    // 这里只显式激活布局，几何始终由布局计算。
    // mainHLayout->activate() 只把三个子部件的几何设置到位，子部件内部的
    // QHBoxLayout 仍在 LayoutRequest 队列里排队（Qt 在子部件尺寸变化时仅
    // postEvent(LayoutRequest)，不保证本帧内重排）——同帧读取或同帧绘制的帧里，
    // newChatButton 的 AlignRight 尚未生效，按钮会停在上一帧按旧宽度算出的坐标上
    // （表现为“按钮出现在预期位置左侧”）。这里把三级布局都在本帧内激活，
    // 使 setSize() 返回时 newChatButton 的几何已经是最终值
    mainHLayout->activate();
    funLeftSubHLayout->activate();
    funMidSubHLayout->activate();
    funRightSubHLayout->activate();

    // qDebug() << "FunWidget size:" << size();
    // qDebug() << "funLeftSubWidget geometry:" << funLeftSubWidget->geometry();
    // qDebug() << "funMidSubWidget geometry:" << funMidSubWidget->geometry();
    // qDebug() << "funRightSubWidget geometry:" << funRightSubWidget->geometry();
    // qDebug() << "funLeftSubHLayout contentsmargins:" << funLeftSubHLayout->contentsMargins();
    // qDebug() << "funMidSubHLayout contentsmargins:" << funMidSubHLayout->contentsMargins();
    // qDebug() << "funRightSubHLayout contentsmargins:" << funRightSubHLayout->contentsMargins();
    // qDebug() << "chatRecordsButton geometry:" << chatRecordsButton->geometry();
    // qDebug() << "titleLabel geometry:" << titleLabel->geometry();
    // qDebug() << "newChatButton geometry:" << newChatButton->geometry();
}

void FunWidget::saveWidgetSize()
{
    // 构造函数中的尺寸即 144 DPI 基准值；updateSize 后按当前已应用 DPI 折算回基准
    widgetSizeDict["this"] = DpiUtil::toBaseSize(size(), appliedDpi);
    widgetSizeDict["chatRecordsButton"] = DpiUtil::toBaseSize(chatRecordsButton->size(), appliedDpi);
    widgetSizeDict["chatRecordsButton iconSize"] =
            DpiUtil::toBaseSize(chatRecordsButton->iconSize(), appliedDpi);
    widgetSizeDict["funLeftSubWidget"] = DpiUtil::toBaseSize(funLeftSubWidget->size(), appliedDpi);
    widgetSizeDict["funLeftSubHLayout contentsMargins"] =
            QVariant::fromValue(DpiUtil::toBaseMargins(funLeftSubHLayout->contentsMargins(),
                                                      appliedDpi));
    widgetSizeDict["titleLabel"] = DpiUtil::toBaseSize(titleLabel->size(), appliedDpi);
    widgetSizeDict["funMidSubWidget"] = DpiUtil::toBaseSize(funMidSubWidget->size(), appliedDpi);
    widgetSizeDict["funMidSubHLayout contentsMargins"] =
            QVariant::fromValue(DpiUtil::toBaseMargins(funMidSubHLayout->contentsMargins(),
                                                      appliedDpi));
    widgetSizeDict["newChatButton"] = DpiUtil::toBaseSize(newChatButton->size(), appliedDpi);
    widgetSizeDict["newChatButton iconSize"] =
            DpiUtil::toBaseSize(newChatButton->iconSize(), appliedDpi);
    widgetSizeDict["funRightSubWidget"] = DpiUtil::toBaseSize(funRightSubWidget->size(), appliedDpi);
    widgetSizeDict["funRightSubHLayout contentsMargins"] =
            QVariant::fromValue(DpiUtil::toBaseMargins(funRightSubHLayout->contentsMargins(),
                                                      appliedDpi));
    widgetSizeDict["mainHLayout contentsMargins"] =
            QVariant::fromValue(DpiUtil::toBaseMargins(mainHLayout->contentsMargins(), appliedDpi));
}

void FunWidget::resetWidgetSize()
{
    setSize();
    saveWidgetSize();
}

void FunWidget::updateSize(qreal curDpi, qreal lastDpi)
{
    // 缓存值均为 144 DPI 基准：按目标 DPI 绝对换算（不再用 curDpi/lastDpi 叠乘，
    // 避免取整误差随跨屏次数累积）
    Q_UNUSED(lastDpi);
    if (curDpi <= 0)
        curDpi = DpiUtil::kBaseDpi;

    setFixedSize(DpiUtil::scaleSize(widgetSizeDict["this"].value<QSize>(), curDpi));
    chatRecordsButton->setFixedSize(
            DpiUtil::scaleSize(widgetSizeDict["chatRecordsButton"].value<QSize>(), curDpi));
    chatRecordsButton->setIconSize(
            DpiUtil::scaleSize(widgetSizeDict["chatRecordsButton iconSize"].value<QSize>(), curDpi));
    chatRecordsButton->setStyleSheet(QString("QPushButton{ border:none; border-radius:%1px; }"
                                             "QPushButton:hover{ background:#d0d0d0; }")
                                             .arg(chatRecordsButton->width() / 2));

    // 三个子部件是 mainHLayout 的布局项，不能设置固定尺寸：固定尺寸（min = max）
    // 会强制被布局满足，右侧子部件将超出 FunWidget 右边缘（与 setSize 中手工
    // resize 同类问题）。缩放内部控件与边距后，几何交给布局重新计算。
    funLeftSubHLayout->setContentsMargins(DpiUtil::scaleMargins(
            widgetSizeDict["funLeftSubHLayout contentsMargins"].value<QMargins>(), curDpi));

    titleLabel->setFixedSize(DpiUtil::scaleSize(widgetSizeDict["titleLabel"].value<QSize>(), curDpi));
    funMidSubHLayout->setContentsMargins(0, 0, 0, 0);

    // 标题字体按新像素字号重设（像素字号不随屏 DPI 自动变化，需显式重设）
    if (!titleFont.family().isEmpty()) {
        titleFont.setPixelSize(appContext->titleFontPixelSize());
        titleLabel->setFont(titleFont);
    }

    newChatButton->setFixedSize(DpiUtil::scaleSize(widgetSizeDict["newChatButton"].value<QSize>(), curDpi));
    newChatButton->setIconSize(
            DpiUtil::scaleSize(widgetSizeDict["newChatButton iconSize"].value<QSize>(), curDpi));
    newChatButton->setStyleSheet(QString("QPushButton{ border:none; border-radius:%1px; }"
                                         "QPushButton:hover{ background:#d0d0d0; }")
                                         .arg(newChatButton->width() / 2));

    funRightSubHLayout->setContentsMargins(DpiUtil::scaleMargins(
            widgetSizeDict["funRightSubHLayout contentsMargins"].value<QMargins>(), curDpi));
    mainHLayout->setContentsMargins(0, 0, 0, 0);

    // 记录缓存当前对应的 DPI：后续 saveWidgetSize 据此把像素值折算回 144 基准
    appliedDpi = curDpi;

    // 尺寸与内部控件已按新 DPI 重设：立即激活各级布局，使本次 updateSize()
    // 返回时三个子部件的几何已是最终值（跨屏来回切换时 1200x60 ↔ 800x40
    // 严格幂等，不依赖下一帧 LayoutRequest）
    setSize();
}
