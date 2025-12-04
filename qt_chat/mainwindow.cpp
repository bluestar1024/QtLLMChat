#include "mainwindow.h"
#include <QDebug>

const QString imagesDir = ":/images";
const QString fontFilePath = ":/font/msyhl.ttc";
const QString mathjaxScriptPath = ":/mathjax/es5/tex-mml-chtml.js";
const int windowFontPointSize = 10;
const int windowFontPixelSize = 20;
const int titleFontPixelSize = 28;
const int buttonFontPointSize = 9;
const QColor textEditFullBGColor = QColor(224, 224, 224);
const QColor textEditFullBGTColor = QColor(224, 224, 224, 0);
const QColor textEditFullBTColor = QColor(100, 100, 100);
const QColor textEditFullBColor = QColor(100, 100, 100, 0);
const QString testText = R"(
一级标题
========
二级标题
--------
------------------
# 一级标题
## 二级标题
### 三级标题
这是一个段落，包含一些 *Markdown* 语法。
*斜体文本*
_斜体文本_
**粗体文本**
__粗体文本__
***粗斜体文本***
___粗斜体文本___
~~删除线文本~~
<u>带下划线文本</u>
苹果10$，梨子20$，香蕉30$，橘子40$。
苹果10\\$，梨子20\\$，香蕉30\\$，橘子40\\$。
$\alpha$ $\beta$ $\gamma$ $\delta$ $\epsilon$ $\zeta$ $\eta$ $\theta$ $\iota$ $\kappa$ $\lambda$ $\mu$ $\nu$ $\xi$ $\omicron$ $\pi$ $\rho$ $\sigma$ $\tau$ $\upsilon$ $\phi$ $\chi$ $\psi$ $\omega$ $\Gamma$ $\Delta$ $\Theta$ $\Lambda$ $\Xi$ $\Pi$ $\Sigma$ $\Upsilon$ $\Phi$ $\Psi$ $\Omega$
创建脚注格式类似这样 [^RUNOOB]。
[^RUNOOB]: 菜鸟教程 -- 学的不仅是技术，更是梦想！！！

******************
### 无序列表
* 第一项
* 第二项
* 第三项

+ 第一项
+ 第二项
+ 第三项

- 第一项
- 第二项
- 第三项

### 有序列表
1. 第一项
2. 第二项
3. 第三项

### 列表嵌套
1. 第一项
    - 第一项嵌套的第一个元素
    - 第一项嵌套的第二个元素
2. 第二项
    1. 第二项嵌套的第一个元素
    2. 第二项嵌套的第二个元素
******************
### 区块
> 菜鸟教程
> 学的不仅是技术更是梦想

> 最外层
> > 第一层嵌套
> > > 第二层嵌套

> 区块中使用列表
> 1. 第一项
> 2. 第二项
> + 第一项
> + 第二项
> + 第三项

列表中使用区块
* 第一项
    > 菜鸟教程
    > 学的不仅是技术更是梦想
* 第二项
******************
### 代码
`printHelloWorld()`函数

    def printHelloWorld():
        print('hello world')

```python
def printHelloWorld():
    print('hello world')
```
******************
### 链接
这是一个链接 [菜鸟教程](https://www.runoob.com)
这个链接用 1 作为网址变量 [Google][1]
这个链接用 runoob 作为网址变量 [Runoob][runoob]
然后在文档的结尾为变量赋值（网址）

[1]: http://www.google.com/
[runoob]: http://www.runoob.com/
******************
### 图片
![RUNOOB 图标](https://static.jyshare.com/images/runoob-logo.png "RUNOOB")

这个链接用 2 作为网址变量 [RUNOOB][2]
然后在文档的结尾为变量赋值（网址）

[2]: https://static.jyshare.com/images/runoob-logo.png

<img src="https://static.jyshare.com/images/runoob-logo.png" width="25%">

******************
### 表格
|  表头  |  表头  |  表头  |
| :----- | -----: | :----: |
| 单元格 | 单元格 | 单元格 |
| 单元格 | 单元格 | 单元格 |
******************
### LaTeX公式：
这是一个行内公式 $E=mc^2$ 的示例。
行间公式：
$$\sum_{i=1}^n a_i$$
$$\frac{a}{b} = c$$
积分公式：
$$\int_{a}^{b} {f(x)} \, \mathrm{d}x = F(b) - F(a)$$
微分公式：
$$\frac{d}{dx} e^x = e^x$$
)";

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setMinimumSize(1110, 795);
    resize(1220, 820);
    setAttribute(Qt::WA_TranslucentBackground, true);
    setWindowFlags(Qt::FramelessWindowHint | Qt::CustomizeWindowHint);
    setMouseTracking(true);

    mouseLeftButtonIsPress = false;
    regionDir = RegionEnum::MIDDLE;
    padding = 2;
//    titleWidgetInit();
    chatFun = new FunWidget();
    chatFun->connectChatRecordsButtonClick(this, &MainWindow::showChatRecords);
    chatFun->connectNewChatButtonClick(this, &MainWindow::newChat);
    chatShow = new ListWidget();
    chatShowWidget = new Widget();
    chatShowWidget->setMinimumHeight(244);
    chatShowWidget->resize(1200, 500);
    chatShowWidget->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    chatShowVLayout = new QVBoxLayout(chatShowWidget);
    chatShowVLayout->addWidget(chatShow);
    chatShowVLayout->setContentsMargins(20, 4, 10, 16);
    chatInput = new TextEditFull();
    chatInput->connectSendButtonClick(this, &MainWindow::sendMessage);
    chatInputWidget = new Widget();
    chatInputWidget->setMinimumHeight(100);
    chatInputWidget->resize(1200, 200);
    chatInputWidget->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    chatInputVLayout = new QVBoxLayout(chatInputWidget);
    chatInputVLayout->addWidget(chatInput);
    chatInputVLayout->setContentsMargins(20, 0, 20, 20);
    splitter = new Splitter(Qt::Vertical);
    splitter->resize(1200, 700);
    splitter->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    splitter->setChildrenCollapsible(false);
    splitter->addWidget(chatShowWidget);
    splitter->addWidget(chatInputWidget);
    splitter->setContentsMargins(0, 0, 0, 0);
    splitter->setStretchFactor(0, 1);
    splitter->setStretchFactor(1, 0);
    splitter->setHandleWidth(0);
    contentWidget = new Widget();
    contentWidget->resize(1200, 760);
    contentWidget->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    contentVLayout = new QVBoxLayout(contentWidget);
    contentVLayout->addWidget(chatFun);
    contentVLayout->addWidget(splitter);
    contentVLayout->setContentsMargins(0, 0, 0, 0);
    contentVLayout->setSpacing(0);
    contentVLayout->setStretch(0, 0);
    contentVLayout->setStretch(1, 1);
    mainWidget = new Frame(this);
    mainWidget->setGeometry(10, 10, width() - 20, height() - 20);
    mainWidget->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    mainWidget->setObjectName("mainWidget");
    mainWidget->setStyleSheet(
        "#mainWidget {"
        "    border-radius: 16px;"
        "    background-color: #F0F0F0;"
        "}"
    );
    mainVLayout = new QVBoxLayout(mainWidget);
//    mainVLayout->addWidget(titleWidget);
    mainVLayout->addWidget(contentWidget);
    mainVLayout->setContentsMargins(0, 0, 0, 0);
    mainVLayout->setSpacing(0);
    mainVLayout->setStretch(0, 0);
    mainVLayout->setStretch(1, 1);
    QGraphicsDropShadowEffect *shadow = new QGraphicsDropShadowEffect(this);
    shadow->setBlurRadius(20);
    shadow->setOffset(0, 0);
    shadow->setColor(QColor(0, 0, 0, 150));
    setGraphicsEffect(shadow);
    messageWidgetList.clear();

//    settingWidgetInit();
//    chatRecordsWidget = ChatRecordsWidget(mainWidget);
//    chatRecordsWidget.connectSettingButtonClick(settingButtonClicked);
//    chatRecordsWidget.connectLineEditTextChanged(showSearchRecords);
//    chatRecordsWidget.connectSearchButtonClick(showSearchRecords);
//    chatRecordsWidget.connectClearAllButtonClick(clearAllChatRecords);
//    chatRecordsWidget.connectListItemClick(generateChatRecord);
//    chatRecordsWidget.move(-self.chatRecordsWidget.width(), self.titleWidget.height());
//    curChatFile = '';

//    chatRecordsAnimationMove = new QPropertyAnimation(chatRecordsWidget, "geometry");
//    chatRecordsAnimationMove->setDuration(1000);
//    chatRecordsAnimationMove->setEasingCurve(QEasingCurve::OutQuad);
//    connect(chatRecordsAnimationMove, &QPropertyAnimation::valueChanged, this, &MainWindow::chatRecordsUiAnimationMove);
//    connect(chatRecordsAnimationMove, &QPropertyAnimation::finished, this, &MainWindow::chatRecordsUiMoveFinished);

//    chatRecordsWidgetIsOpen = false;
//    emptyTextLabel = new PrintLabel("文本不能为空", this);
//    emptyTextLabel->move((width() - emptyTextLabel->width()) / 2,
//                         titleWidget->height() + chatFun->height() + chatShowWidget->height() + 10);
//    emptyTextLabel->raise();
//    emptyTextLabel->hide();
//    textCopyLabel = new PrintLabel("文本复制成功", this);
//    textCopyLabel->move((width() - textCopyLabel->width()) / 2,
//                        titleWidget->height() + chatFun->height() + chatShowWidget->height() + 10);
//    textCopyLabel->raise();
//    textCopyLabel->hide();
    isRegenerate = false;
    isRegenerateFirst = true;
    isSetTexting = false;
    pushButtonIsPress = false;
    lastScreen = curScreen = screen();
    initDpi = lastDpi = curDpi = curScreen->logicalDotsPerInch();
    screenChanged = false;
    thinkExpandedList.clear();
    thinkTimeLengthList.clear();
    isSending = false;
    isContinueShow = true;
    isScreenMax = false;
    isScreenHalf = false;
    lastNormalGeometry = geometry();
    uiRectWidth = width();
    uiRectHeight = height();
    isChangeRectFirst = false;
    screens = QApplication::screens();
    for (QScreen *screen : screens) {
        connect(screen, &QScreen::logicalDotsPerInchChanged, this, &MainWindow::onDpiChanged);
    }
    isDpiChanged = false;
    widgetSizeDict["MainWindow"] = size();
    widgetSizeDict["MainWindow minimumSize"] = minimumSize();
    widgetSizeDict["mainWidget"] = mainWidget->size();
    widgetSizeDict["mainWidget x"] = mainWidget->x();
    widgetSizeDict["mainWidget y"] = mainWidget->y();
    avoidRepeatSelfFun = false;
}

MainWindow::~MainWindow()
{
}

void MainWindow::onDpiChanged()
{

}

void MainWindow::messageWidgetResize()
{

}

void MainWindow::sendMessage()
{
    if (!isSending) {
        QList<QVariantMap> context = {};
        QString text = chatInput->toPlainText().trimmed();
        if (!text.isEmpty()) {
            for (auto *w : messageWidgetList) {
                QVariantMap m;
                m["role"]    = w->getIsUser() ? "user" : "assistant";
                m["content"] = w->getText();
                context.append(m);
            }
            thinkTimeLengthList.append(0);
            messageSendWidget = new MessageWidget(
                text,
                [this](){ textCopy(); },
                [this](){ messageRenewResponse(); },
                chatShow,
                thinkTimeLengthList,
                messageWidgetList.size(),
                true,
                true,
                chatShow->width() * 3 / 4);
            messageSendWidget->updateFunWidgetSize(curDpi, initDpi);
            messageSendWidget->connectResizeFinished(this, &MainWindow::messageWidgetResize);
            messageSendWidget->connectSetTexting(this, &MainWindow::getSetTexting);
            messageSendWidget->connectExecuteNext(this, &MainWindow::onExecuteNext);
            messageSendWidget->toggleWidget();
            messageWidgetList.append(messageSendWidget);

            itemSendWidget = new ItemWidget(this);
            itemSendHLayout = new QHBoxLayout(itemSendWidget);
            itemSendHLayout->addWidget(messageSendWidget);
            itemSendWidget->setFixedSize(chatShow->width(),
                                         messageSendWidget->height() + 10);
            itemSendHLayout->setContentsMargins(
                itemSendWidget->width() - messageSendWidget->width() - 25,
                5, 25, 5);

            sendItem = new QListWidgetItem(chatShow);
            sendItem->setSizeHint(QSize(chatShow->width(),
                                        messageSendWidget->height() + 10));
            chatShow->setItemWidget(sendItem, itemSendWidget);

            thread = new MessageThread(text, context);
            chatInput->clearText();
            chatInput->setSending(true);
            isSending = true;
            qDebug() << "sendMessage:" << text;
        } else {
//            emptyTextLabel->printStart();
        }
    } else {
        if (thread) thread->stop();
        isSending = false;
        if (messageRecvWidget) messageRecvWidget->breakHandle();
    }
}

void MainWindow::onExecuteNext()
{
    qDebug() << "onExecuteNext";
    QTimer::singleShot(50, this, &MainWindow::startThread);
}

void MainWindow::startThread()
{
    connect(thread, &QThread::started, this, &MainWindow::messageStart);
    connect(thread, &MessageThread::newMessage, this, &MainWindow::recvMessage);
    connect(thread, &QThread::finished, this, &MainWindow::messageFinish);
    thread->start();
    qDebug() << "startThread";
}

void MainWindow::messageStart()
{
    Message.clear();
    int i = messageWidgetList.size() - 1;
    if (i != 0) {
        if (messageWidgetList[i]->getIsUser())
            messageWidgetList[i - 1]->removeRenewResponseButton();
        else
            messageWidgetList[i]->removeRenewResponseButton();
    }

    thinkTimeLengthList.append(0);
    messageRecvWidget = new MessageWidget(
        Message,
        [this](){ textCopy(); },
        [this](){ messageRenewResponse(); },
        chatShow,
        thinkTimeLengthList,
        messageWidgetList.size(),
        false,
        true,
        chatShow->width() * 3 / 4);
    messageRecvWidget->connectResizeFinished(this, &MainWindow::messageWidgetResize);
    messageRecvWidget->connectSetTexting(this, &MainWindow::getSetTexting);
    messageWidgetList.append(messageRecvWidget);

    itemRecvWidget = new ItemWidget(this);
    itemRecvHLayout = new QHBoxLayout(itemRecvWidget);
    itemRecvHLayout->addWidget(messageRecvWidget);
    itemRecvWidget->setFixedSize(chatShow->width(),
                                 messageRecvWidget->height() + 10);
    itemRecvHLayout->setContentsMargins(
        0, 5,
        itemRecvWidget->width() - messageRecvWidget->width(), 5);

    recvItem = new QListWidgetItem(chatShow);
    recvItem->setSizeHint(QSize(chatShow->width(),
                                messageRecvWidget->height() + 10));
    chatShow->setItemWidget(recvItem, itemRecvWidget);

    first = true;
    qDebug() << "messageStart";
}

void MainWindow::recvMessage(const QString &text)
{
    qDebug() << "recvMessage:" << text;
    if (first) {
        first = false;
        if (text.startsWith("\n "))
            Message = text.mid(2);
        else
            Message = text;
    } else {
        Message += text;
    }

    if (isContinueShow) {
        messageRecvWidget->setText(Message);
        itemRecvWidget->setFixedSize(chatShow->width(),
                                     messageRecvWidget->height() + 10);
        itemRecvHLayout->setContentsMargins(
            0, 5,
            itemRecvWidget->width() - messageRecvWidget->width(), 5);
        recvItem->setSizeHint(QSize(chatShow->width(),
                                    messageRecvWidget->height() + 10));
    }
}

void MainWindow::messageFinish()
{
    chatInput->setSending(false);
    messageRecvWidget->removeLoadingWidget();
    messageRecvWidget->updateFunWidgetSize(curDpi, initDpi);
    messageRecvWidget->toggleWidget();

    itemRecvWidget->setFixedSize(chatShow->width(),
                                 messageRecvWidget->height() + 10);
    itemRecvHLayout->setContentsMargins(
        0, 5,
        itemRecvWidget->width() - messageRecvWidget->width(), 5);
    recvItem->setSizeHint(QSize(chatShow->width(),
                                messageRecvWidget->height() + 10));

    if (Message.isEmpty()) {
        delete messageWidgetList.takeLast();
        int last = chatShow->count() - 1;
        QWidget *itemWidget = chatShow->itemWidget(chatShow->item(last));
        if (itemWidget) itemWidget->deleteLater();
        QListWidgetItem *lastItem = chatShow->takeItem(last);
        delete lastItem;
        messageRenewResponse();
    }
    isSending = false;
}

void MainWindow::textCopy()
{

}

void MainWindow::messageRenewResponse()
{

}

void MainWindow::getSetTexting(bool state)
{
    isSetTexting = state;
}

void MainWindow::showChatRecords()
{

}

void MainWindow::newChat()
{

}

// QScreen* MainWindow::getScreenForWidget(const QWidget* widget)
// {
//     QDesktopWidget* desktop = QApplication::desktop();
//     int screenNumber = desktop->screenNumber(widget);
//     if (screenNumber != -1) {
//         QList<QScreen*> screens = QApplication::screens();
//         if (screenNumber < screens.size()) {
//             return screens.at(screenNumber);
//         }
//     }
//     return QApplication::primaryScreen();
// }
