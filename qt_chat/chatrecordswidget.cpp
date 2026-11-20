#include "chatrecordswidget.h"
#include "appcontext.h"
#include "dpiutil.h"

ChatRecordsWidget::ChatRecordsWidget(AppContext *appContext, QWidget *parent)
    : QWidget(parent),
      appContext(appContext),
      settingButton(nullptr),
      buttonWidget(nullptr),
      buttonVLayout(nullptr),
      label(nullptr),
      headWidget(nullptr),
      headHLayout(nullptr),
      lineEdit(nullptr),
      clearAllButton(nullptr),
      searchWidget(nullptr),
      searchHLayout(nullptr),
      listWidget(nullptr),
      mainWidget(nullptr),
      mainVLayout(nullptr),
      appliedDpi(DpiUtil::kBaseDpi),
      chatRecordItem(nullptr)
{
    resize(400, 760);
    setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);

    // 字体族由 AppContext 统一注册缓存，直接取用（不再逐个 addApplicationFont）
    fontFamily = appContext->fontFamily();
    if (!fontFamily.isEmpty()) {
        font = QFont(fontFamily);
        font.setPixelSize(this->appContext->windowFontPixelSize());
    }

    settingButton = new PushButton(this->appContext, "设置");
    settingButton->setFixedSize(44, 44);
    settingImagesPath = this->appContext->imagesDir() + "/setting.png";
    settingButton->setIcon(QIcon(settingImagesPath));
    settingButton->setIconSize(QSize(30, 30));
    settingButton->setStyleSheet("QPushButton{"
                                 "    border: none;"
                                 "    border-radius: 22px;"
                                 "}"
                                 "QPushButton:hover{"
                                 "    background: #b0b0b0;"
                                 "}");

    buttonWidget = new Widget();
    buttonWidget->setFixedSize(44, 50);

    buttonVLayout = new QVBoxLayout();
    buttonWidget->setLayout(buttonVLayout);
    buttonVLayout->addWidget(settingButton);
    buttonVLayout->setAlignment(Qt::AlignTop);
    buttonVLayout->setContentsMargins(0, 0, 0, 6);

    label = new QLabel();
    label->resize(width() - buttonWidget->width() - 40, 50);
    label->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);

    // 标题字体用成员 titleFont 记录（字体族 + 加粗），updateSize 跨屏缩放时按新
    // 像素字号重设，保证标题与列表项同步缩放
    titleFont = QFont(fontFamily);
    titleFont.setPixelSize(this->appContext->titleFontPixelSize());
    titleFont.setBold(true);
    label->setFont(titleFont);
    label->setText("聊天历史");
    label->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);

    headWidget = new Widget();
    headWidget->resize(width() - 40, 50);
    headWidget->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);

    headHLayout = new QHBoxLayout();
    headWidget->setLayout(headHLayout);
    headHLayout->addWidget(label);
    headHLayout->addWidget(buttonWidget);
    headHLayout->setContentsMargins(0, 0, 0, 0);
    headHLayout->setStretch(0, 1);
    headHLayout->setStretch(1, 0);

    lineEdit = new LineEdit(appContext);

    clearAllButton = new PushButton(appContext, "删除所有记录");
    clearAllButton->setFixedSize(32, 32);
    clearAllButton->setIconSize(QSize(30, 30));
    clearAllImagesPath = appContext->imagesDir() + "/clearAll.png";
    clearAllHoverImagesPath = appContext->imagesDir() + "/clearAll_hover.png";

    clearAllButton->setStyleSheet(QString("QPushButton{"
                                          "    border: none;"
                                          "    border-radius: 5px;"
                                          "    background: #e0e0e0;"
                                          "    image: url(\"%1\");"
                                          "}"
                                          "QPushButton:hover{"
                                          "    background: #b8b8b8;"
                                          "    image: url(\"%2\");"
                                          "}")
                                          .arg(clearAllImagesPath, clearAllHoverImagesPath));

    searchWidget = new QWidget();
    searchWidget->resize(width() - 40, 32);
    searchWidget->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);

    searchHLayout = new QHBoxLayout();
    searchWidget->setLayout(searchHLayout);
    searchHLayout->addWidget(lineEdit);
    searchHLayout->addWidget(clearAllButton);
    searchHLayout->setContentsMargins(0, 0, 0, 0);
    searchHLayout->setSpacing(10);
    searchHLayout->setStretch(0, 1);
    searchHLayout->setStretch(1, 0);

    listWidget = new QListWidget();
    listWidget->setFixedSize(width() - 40, height() - 112);
    listWidget->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    listWidget->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
    listWidget->setStyleSheet("QListWidget{"
                              "    border: none;"
                              "    background: transparent;"
                              "}"
                              "QScrollBar{"
                              "    width: 20px;"
                              "}");

    mainWidget = new Widget(this);
    mainWidget->resize(1200 / 3, 760);
    mainWidget->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);

    mainVLayout = new QVBoxLayout();
    mainWidget->setLayout(mainVLayout);
    mainVLayout->addWidget(headWidget);
    mainVLayout->addWidget(searchWidget);
    mainVLayout->addWidget(listWidget);
    mainVLayout->setContentsMargins(20, 10, 20, 0);
    mainVLayout->setSpacing(10);
    mainVLayout->setStretch(0, 0);
    mainVLayout->setStretch(1, 0);
    mainVLayout->setStretch(2, 1);

    setMouseTracking(true);

    // 列表项高度以 144 DPI 基准记录，取用时按当前已应用 DPI 换算
    widgetSizeDict["chatRecordItem height"] = 60;
    saveWidgetSize();
}

ChatRecordsWidget::~ChatRecordsWidget() { }

void ChatRecordsWidget::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    QPainterPath path;
    path.setFillRule(Qt::WindingFill);
    path.addRoundedRect(rect().x(), rect().y(), rect().width(), rect().height(), 16, 16);
    path.addRect(rect().x(), rect().y(), 16, 16);
    path.addRect(rect().width() - 16, rect().y(), 16, 16);
    path.addRect(rect().width() - 16, rect().height() - 16, 16, 16);

    QBrush brush(Qt::SolidPattern);
    brush.setColor(QColor(208, 208, 208));

    painter.setPen(Qt::NoPen);
    painter.setBrush(brush);
    painter.drawPath(path.simplified());
}

void ChatRecordsWidget::mouseMoveEvent(QMouseEvent *event)
{
    QWidget::mouseMoveEvent(event);
    event->ignore();
}

void ChatRecordsWidget::setSize()
{
    QMargins mainMargins = mainVLayout->contentsMargins();
    label->setFixedSize(width() - mainMargins.left() - mainMargins.right() - buttonWidget->width(),
                        label->height());
    headWidget->setFixedSize(width() - mainMargins.left() - mainMargins.right(),
                             headWidget->height());
    lineEdit->setFixedSize(width() - mainMargins.left() - mainMargins.right()
                                   - clearAllButton->width() - searchHLayout->spacing(),
                           lineEdit->height());
    lineEdit->resetWidgetSize();
    searchWidget->setFixedSize(width() - mainMargins.left() - mainMargins.right(),
                               searchWidget->height());
    listWidget->setFixedSize(width() - mainMargins.left() - mainMargins.right(),
                             height() - headWidget->height() - searchWidget->height()
                                     - mainMargins.top() - 2 * mainVLayout->spacing());
    mainWidget->resize(width(), height());
}

void ChatRecordsWidget::saveWidgetSize()
{
    // 按当前已应用的 DPI 折算回 144 DPI 基准
    widgetSizeDict["self"] = DpiUtil::toBaseSize(size(), appliedDpi);
    widgetSizeDict["settingButton"] = DpiUtil::toBaseSize(settingButton->size(), appliedDpi);
    widgetSizeDict["settingButton iconSize"] =
            DpiUtil::toBaseSize(settingButton->iconSize(), appliedDpi);
    widgetSizeDict["buttonWidget"] = DpiUtil::toBaseSize(buttonWidget->size(), appliedDpi);
    widgetSizeDict["buttonVLayout contentsMargins"] =
            QVariant::fromValue(DpiUtil::toBaseMargins(buttonVLayout->contentsMargins(),
                                                      appliedDpi));
    widgetSizeDict["label"] = DpiUtil::toBaseSize(label->size(), appliedDpi);
    widgetSizeDict["headWidget"] = DpiUtil::toBaseSize(headWidget->size(), appliedDpi);
    widgetSizeDict["headHLayout contentsMargins"] =
            QVariant::fromValue(DpiUtil::toBaseMargins(headHLayout->contentsMargins(), appliedDpi));
    widgetSizeDict["lineEdit"] = DpiUtil::toBaseSize(lineEdit->size(), appliedDpi);
    widgetSizeDict["clearAllButton"] = DpiUtil::toBaseSize(clearAllButton->size(), appliedDpi);
    widgetSizeDict["clearAllButton iconSize"] =
            DpiUtil::toBaseSize(clearAllButton->iconSize(), appliedDpi);
    widgetSizeDict["searchWidget"] = DpiUtil::toBaseSize(searchWidget->size(), appliedDpi);
    widgetSizeDict["searchHLayout contentsMargins"] =
            QVariant::fromValue(DpiUtil::toBaseMargins(searchHLayout->contentsMargins(),
                                                      appliedDpi));
    widgetSizeDict["searchHLayout spacing"] =
            DpiUtil::toBasePx(searchHLayout->spacing(), appliedDpi);
    widgetSizeDict["listWidget"] = DpiUtil::toBaseSize(listWidget->size(), appliedDpi);
    widgetSizeDict["mainWidget"] = DpiUtil::toBaseSize(mainWidget->size(), appliedDpi);
    widgetSizeDict["mainVLayout contentsMargins"] =
            QVariant::fromValue(DpiUtil::toBaseMargins(mainVLayout->contentsMargins(), appliedDpi));
    widgetSizeDict["mainVLayout spacing"] =
            DpiUtil::toBasePx(mainVLayout->spacing(), appliedDpi);
}

void ChatRecordsWidget::resetWidgetSize()
{
    setSize();
    saveWidgetSize();
}

void ChatRecordsWidget::updateSize(qreal curDpi, qreal lastDpi)
{
    // 缓存值均为 144 DPI 基准：按目标 DPI 绝对换算（不再用 curDpi/lastDpi 叠乘，
    // 避免取整误差随跨屏次数累积）
    Q_UNUSED(lastDpi);
    if (curDpi <= 0)
        curDpi = DpiUtil::kBaseDpi;

    resize(DpiUtil::scaleSize(widgetSizeDict["self"].value<QSize>(), curDpi));
    settingButton->setFixedSize(
            DpiUtil::scaleSize(widgetSizeDict["settingButton"].value<QSize>(), curDpi));
    settingButton->setIconSize(
            DpiUtil::scaleSize(widgetSizeDict["settingButton iconSize"].value<QSize>(), curDpi));
    settingButton->setStyleSheet(QString("QPushButton{"
                                         "    border: none;"
                                         "    border-radius: %1px;"
                                         "}"
                                         "QPushButton:hover{"
                                         "    background: #b0b0b0;"
                                         "}")
                                         .arg(settingButton->width() / 2));
    buttonWidget->setFixedSize(DpiUtil::scaleSize(widgetSizeDict["buttonWidget"].value<QSize>(), curDpi));
    buttonVLayout->setContentsMargins(DpiUtil::scaleMargins(
            widgetSizeDict["buttonVLayout contentsMargins"].value<QMargins>(), curDpi));
    label->setFixedSize(DpiUtil::scaleSize(widgetSizeDict["label"].value<QSize>(), curDpi));
    headWidget->setFixedSize(DpiUtil::scaleSize(widgetSizeDict["headWidget"].value<QSize>(), curDpi));
    headHLayout->setContentsMargins(0, 0, 0, 0);
    lineEdit->updateSize(curDpi, lastDpi);
    clearAllButton->setFixedSize(
            DpiUtil::scaleSize(widgetSizeDict["clearAllButton"].value<QSize>(), curDpi));
    clearAllButton->setIconSize(
            DpiUtil::scaleSize(widgetSizeDict["clearAllButton iconSize"].value<QSize>(), curDpi));
    searchWidget->setFixedSize(
            DpiUtil::scaleSize(widgetSizeDict["searchWidget"].value<QSize>(), curDpi));
    searchHLayout->setContentsMargins(0, 0, 0, 0);
    searchHLayout->setSpacing(
            DpiUtil::scalePx(widgetSizeDict["searchHLayout spacing"].value<int>(), curDpi, 0));
    listWidget->setFixedSize(DpiUtil::scaleSize(widgetSizeDict["listWidget"].value<QSize>(), curDpi));
    mainWidget->resize(DpiUtil::scaleSize(widgetSizeDict["mainWidget"].value<QSize>(), curDpi));
    mainVLayout->setContentsMargins(DpiUtil::scaleMargins(
            widgetSizeDict["mainVLayout contentsMargins"].value<QMargins>(), curDpi));
    mainVLayout->setSpacing(
            DpiUtil::scalePx(widgetSizeDict["mainVLayout spacing"].value<int>(), curDpi, 0));

    // 列表项高度保持 144 基准不就地修改（否则每跨一次屏都会就当前像素再乘一次）
    const int itemHeight =
            DpiUtil::scalePx(widgetSizeDict["chatRecordItem height"].value<int>(), curDpi);
    // 列表项字体与标题字体都按新的像素字号重设：列表项字体直接取自
    // appContext（与 WebEngineView 页面同为像素字号，保持视觉一致），
    // 标题字体用成员 titleFont 重设（构造时记录的字体族与加粗属性）
    font.setPixelSize(appContext->windowFontPixelSize());
    titleFont.setPixelSize(appContext->titleFontPixelSize());
    label->setFont(titleFont);
    for (int i = 0; i < listWidget->count(); ++i) {
        QListWidgetItem *item = listWidget->item(i);
        item->setSizeHint(QSize(listWidget->width(), itemHeight));
        item->setFont(font);
    }
    appliedDpi = curDpi;
}

QString ChatRecordsWidget::getLineEditText() const
{
    return lineEdit->text();
}

QListWidgetItem *ChatRecordsWidget::addListItem(const QString &string)
{
    chatRecordItem = new QListWidgetItem(string);
    listWidget->insertItem(0, chatRecordItem);
    chatRecordItem->setSizeHint(QSize(
            listWidget->width(),
            DpiUtil::scalePx(widgetSizeDict["chatRecordItem height"].value<int>(), appliedDpi)));
    font.setPixelSize(appContext->windowFontPixelSize());
    chatRecordItem->setFont(font);
    return chatRecordItem;
}

void ChatRecordsWidget::delAllListItems()
{
    listWidget->clear();
}

void ChatRecordsWidget::listItemSetData(QListWidgetItem *item, const QString &string)
{
    item->setData(Qt::UserRole, QVariant(string));
}

QString ChatRecordsWidget::listItemToString(QListWidgetItem *item) const
{
    return item->data(Qt::UserRole).toString();
}
