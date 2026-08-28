#include "chatrecordswidget.h"
#include "appcontext.h"

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
      chatRecordItem(nullptr)
{
    resize(400, 760);
    setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);

    int fontId = QFontDatabase::addApplicationFont(this->appContext->fontFilePath());
    if (fontId != -1) {
        QStringList fontFamilies = QFontDatabase::applicationFontFamilies(fontId);
        if (!fontFamilies.isEmpty()) {
            fontFamily = fontFamilies.at(0);
            font = QFont(fontFamily);
            font.setPixelSize(this->appContext->windowFontPixelSize());
        }
    }

    settingButton = new PushButton(this->appContext, "设置", 5, 35);
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

    QFont titleFont(fontFamily);
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

    clearAllButton = new PushButton(appContext, "删除所有记录", 25, 35);
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

    widgetSizeDict["self"] = size();
    widgetSizeDict["settingButton"] = settingButton->size();
    widgetSizeDict["settingButton iconSize"] = settingButton->iconSize();
    widgetSizeDict["buttonWidget"] = buttonWidget->size();
    widgetSizeDict["buttonVLayout contentsMargins"] =
            QVariant::fromValue(buttonVLayout->contentsMargins());
    widgetSizeDict["label"] = label->size();
    widgetSizeDict["headWidget"] = headWidget->size();
    widgetSizeDict["headHLayout contentsMargins"] =
            QVariant::fromValue(headHLayout->contentsMargins());
    widgetSizeDict["lineEdit"] = lineEdit->size();
    widgetSizeDict["clearAllButton"] = clearAllButton->size();
    widgetSizeDict["clearAllButton iconSize"] = clearAllButton->iconSize();
    widgetSizeDict["searchWidget"] = searchWidget->size();
    widgetSizeDict["searchHLayout contentsMargins"] =
            QVariant::fromValue(searchHLayout->contentsMargins());
    widgetSizeDict["searchHLayout spacing"] = searchHLayout->spacing();
    widgetSizeDict["listWidget"] = listWidget->size();
    widgetSizeDict["mainWidget"] = mainWidget->size();
    widgetSizeDict["mainVLayout contentsMargins"] =
            QVariant::fromValue(mainVLayout->contentsMargins());
    widgetSizeDict["mainVLayout spacing"] = mainVLayout->spacing();
    widgetSizeDict["chatRecordItem height"] = 60;
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
    widgetSizeDict["self"] = size();
    widgetSizeDict["settingButton"] = settingButton->size();
    widgetSizeDict["settingButton iconSize"] = settingButton->iconSize();
    widgetSizeDict["buttonWidget"] = buttonWidget->size();
    widgetSizeDict["buttonVLayout contentsMargins"] =
            QVariant::fromValue(buttonVLayout->contentsMargins());
    widgetSizeDict["label"] = label->size();
    widgetSizeDict["headWidget"] = headWidget->size();
    widgetSizeDict["headHLayout contentsMargins"] =
            QVariant::fromValue(headHLayout->contentsMargins());
    widgetSizeDict["lineEdit"] = lineEdit->size();
    widgetSizeDict["clearAllButton"] = clearAllButton->size();
    widgetSizeDict["clearAllButton iconSize"] = clearAllButton->iconSize();
    widgetSizeDict["searchWidget"] = searchWidget->size();
    widgetSizeDict["searchHLayout contentsMargins"] =
            QVariant::fromValue(searchHLayout->contentsMargins());
    widgetSizeDict["searchHLayout spacing"] = searchHLayout->spacing();
    widgetSizeDict["listWidget"] = listWidget->size();
    widgetSizeDict["mainWidget"] = mainWidget->size();
    widgetSizeDict["mainVLayout contentsMargins"] =
            QVariant::fromValue(mainVLayout->contentsMargins());
    widgetSizeDict["mainVLayout spacing"] = mainVLayout->spacing();
}

void ChatRecordsWidget::resetWidgetSize()
{
    setSize();
    saveWidgetSize();
}

void ChatRecordsWidget::updateSize(int curDpi, int lastDpi)
{
    if (lastDpi == 0)
        return;
    qreal ratio = qreal(curDpi / lastDpi);

    auto scale = [=](const QSize &s) -> QSize {
        return QSize(qRound(s.width() * ratio), qRound(s.height() * ratio));
    };
    auto scaleMargins = [=](const QMargins &m) -> QMargins {
        return QMargins(qRound(m.left() * ratio), qRound(m.top() * ratio),
                        qRound(m.right() * ratio), qRound(m.bottom() * ratio));
    };

    resize(scale(widgetSizeDict["self"].value<QSize>()));
    settingButton->setFixedSize(scale(widgetSizeDict["settingButton"].value<QSize>()));
    settingButton->setIconSize(scale(widgetSizeDict["settingButton iconSize"].value<QSize>()));
    settingButton->setStyleSheet(QString("QPushButton{"
                                         "    border: none;"
                                         "    border-radius: %1px;"
                                         "}"
                                         "QPushButton:hover{"
                                         "    background: #b0b0b0;"
                                         "}")
                                         .arg(settingButton->width() / 2));
    buttonWidget->setFixedSize(scale(widgetSizeDict["buttonWidget"].value<QSize>()));
    buttonVLayout->setContentsMargins(
            scaleMargins(widgetSizeDict["buttonVLayout contentsMargins"].value<QMargins>()));
    label->setFixedSize(scale(widgetSizeDict["label"].value<QSize>()));
    headWidget->setFixedSize(scale(widgetSizeDict["headWidget"].value<QSize>()));
    headHLayout->setContentsMargins(0, 0, 0, 0);
    lineEdit->updateSize(curDpi, lastDpi);
    clearAllButton->setFixedSize(scale(widgetSizeDict["clearAllButton"].value<QSize>()));
    clearAllButton->setIconSize(scale(widgetSizeDict["clearAllButton iconSize"].value<QSize>()));
    searchWidget->setFixedSize(scale(widgetSizeDict["searchWidget"].value<QSize>()));
    searchHLayout->setContentsMargins(0, 0, 0, 0);
    searchHLayout->setSpacing(qRound(widgetSizeDict["searchHLayout spacing"].value<int>() * ratio));
    listWidget->setFixedSize(scale(widgetSizeDict["listWidget"].value<QSize>()));
    mainWidget->resize(scale(widgetSizeDict["mainWidget"].value<QSize>()));
    mainVLayout->setContentsMargins(
            scaleMargins(widgetSizeDict["mainVLayout contentsMargins"].value<QMargins>()));
    mainVLayout->setSpacing(qRound(widgetSizeDict["mainVLayout spacing"].value<int>() * ratio));

    widgetSizeDict["chatRecordItem height"] =
            qRound(widgetSizeDict["chatRecordItem height"].value<int>() * ratio);
    font.setPixelSize(appContext->windowFontPixelSize());
    for (int i = 0; i < listWidget->count(); ++i) {
        QListWidgetItem *item = listWidget->item(i);
        item->setSizeHint(
                QSize(listWidget->width(), widgetSizeDict["chatRecordItem height"].value<int>()));
        item->setFont(font);
    }
}

QString ChatRecordsWidget::getLineEditText() const
{
    return lineEdit->text();
}

QListWidgetItem *ChatRecordsWidget::addListItem(const QString &string)
{
    chatRecordItem = new QListWidgetItem(string);
    listWidget->insertItem(0, chatRecordItem);
    chatRecordItem->setSizeHint(
            QSize(listWidget->width(), widgetSizeDict["chatRecordItem height"].value<int>()));
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
