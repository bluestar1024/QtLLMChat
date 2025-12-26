#include "messagewidget.h"

#include <QTimer>

MessageWidget::MessageWidget(const QString &text, std::function<void()> copyFun,
                             std::function<void()> renewResponseFun, ListWidget *listWidget,
                             QList<int> &thinkTimeLengthList, int thinkTimeIndex, bool isUser,
                             bool thinkIsExpand, int textMaxWidth, QWidget *parent)
    : QWidget(parent),
      text(text),
      copyFun(copyFun),
      renewResponseFun(renewResponseFun),
      listWidget(listWidget),
      thinkTimeLengthList(thinkTimeLengthList),
      thinkTimeIndex(thinkTimeIndex),
      isUser(isUser),
      thinkIsExpand(thinkIsExpand),
      textMaxWidth(textMaxWidth - 10)
{
    layout = new QHBoxLayout(this);
    layout->setContentsMargins(5, 5, 5, 5);
    textShow = new TextShow(this->text, this->textMaxWidth, this);
    layout->addWidget(textShow);
    // if (!isUser) layout->addStretch();
    // else           layout->setAlignment(Qt::AlignRight);
    setFixedSize(textShow->size() + QSize(10, 0));
    setObjectName("messageWidget");
    setStyleSheet("#messageWidget{"
                  "background: green;"
                  "}");
}

MessageWidget::MessageWidget(const QString &text, std::function<void()> copyFun,
                             std::function<void()> renewResponseFun, bool isUser, int textMaxWidth,
                             QWidget *parent)
    : QWidget(parent),
      text(text),
      copyFun(std::move(copyFun)),
      renewResponseFun(std::move(renewResponseFun)),
      isUser(isUser),
      textMaxWidth(textMaxWidth)
{
    setMouseTracking(true);
    aiUpdateSizeTimer.setSingleShot(true);
    connect(&aiUpdateSizeTimer, &QTimer::timeout, this, &MessageWidget::onAiUpdateSize);

    imageLabel = new ImageLabel(isUser);
    textWidget = new TextWidget(isUser);
    textLayout = new QVBoxLayout(textWidget);
    if (this->isUser)
        textLayout->setContentsMargins(5, 0, 5, 0);
    else
        textLayout->setContentsMargins(15, 5, 15, 5);

    textBoxWidget = new TextBoxWidget();
    textBoxLayout = new QVBoxLayout(textBoxWidget);
    textBoxLayout->setSpacing(0);
    textBoxLayout->setContentsMargins(0, 0, 0, 0);

    funWidget = new QWidget();
    auto *funHLayout = new QHBoxLayout(funWidget);
    funHLayout->setContentsMargins(5, 5, 5, 5);
    copyButton = new CopyButton("复制", 15, 35, this);
    copyButton->setFixedSize(16, 16);
    copyImagesPath = imagesDir + "/copy.png";
    copyHoverImagesPath = imagesDir + "/copy_hover.png";
    copyButton->setStyleSheet(QString("QPushButton{ border-image:url(%1); }"
                                      "QPushButton:hover{ border-image:url(%2); }")
                                      .arg(copyImagesPath, copyHoverImagesPath));
    connect(copyButton, &QPushButton::clicked, this->copyFun);
    funHLayout->addWidget(copyButton);
    copyButton->hide();

    if (!isUser) {
        renewResponseButton = new PushButton("重新生成响应", 25, 35);
        renewResponseButton->setFixedSize(16, 16);
        renewResponseImagesPath = imagesDir + "/renewResponse.png";
        renewResponseHoverImagesPath = imagesDir + "/renewResponse_hover.png";
        renewResponseButton->setStyleSheet(
                QString("QPushButton{ border-image:url(:/icons/renewResponse.png); }"
                        "QPushButton:hover{ border-image:url(:/icons/renewResponse_hover.png); }")
                        .arg(renewResponseImagesPath, renewResponseHoverImagesPath));
        connect(renewResponseButton, &QPushButton::clicked, this->renewResponseFun);
        funHLayout->addWidget(renewResponseButton);
        renewResponseButton->hide();
        funHLayout->setSpacing(10);
        funWidget->setFixedSize(52, 26);
        renewResponseButtonIsRemove = false;
    } else {
        funWidget->setFixedSize(26, 26);
    }

    if (isUser)
        buildUserUi();
    else
        buildAiUi();

    auto *mainHLayout = new QHBoxLayout(this);
    mainHLayout->setContentsMargins(0, 0, 0, 0);
    mainHLayout->setSpacing(5);
    if (isUser) {
        mainHLayout->addWidget(textBoxWidget);
        mainHLayout->addWidget(imageLabel);
    } else {
        mainHLayout->addWidget(imageLabel);
        mainHLayout->addWidget(textBoxWidget);
    }
    setFixedSize(imageLabel->width() + textBoxWidget->width() + 5,
                 qMax(imageLabel->height(), textBoxWidget->height()));
}

MessageWidget::~MessageWidget() { }

void MessageWidget::buildUserUi()
{
    auto *ts = new TextShow(text, true, textMaxWidth - imageLabel->width() - 15, this);
    textLayout->addWidget(ts);
    textWidget->setFixedSize(ts->width() + 10, ts->height());
    textBoxLayout->addWidget(textWidget);
    textBoxLayout->addWidget(funWidget);
    textBoxWidget->setFixedSize(qMax(textWidget->width(), funWidget->width()),
                                textWidget->height() + funWidget->height());
}

void MessageWidget::buildAiUi()
{
    QString think, result;
    parseThinkAndResult(text, think, result, thinkTextIsRecvEnd);

    thinkBackWidget = new ThinkBackWidget(this);
    thinkBackVLayout = new QVBoxLayout(thinkBackWidget);
    thinkBackVLayout->setContentsMargins(30, 0, 15, 0);
    thinkBackVLayout->setSpacing(0);

    if (!think.isEmpty() && !think.contains("</think>")) {
        auto blocks = extractCodeBlocks(think);
        QStringList splitTexts;
        QString temp = think;
        for (const auto &b : blocks) {
            int idx = temp.indexOf(QString("```%1\n").arg(b.language));
            splitTexts.append(temp.left(idx));
            temp = temp.mid(idx + QString("```%1\n").arg(b.language).size() + b.code.size() + 3);
            auto *code =
                    new CodeShow(b.code, b.language, textMaxWidth - imageLabel->width() - 80, this);
            code->connectCodeCopyButtonClick(copyFun);
            thinkCodeShowList.append(code);
        }
        splitTexts.append(temp);
        for (const auto &t : splitTexts) {
            if (!t.isEmpty())
                thinkTextShowList.append(
                        new TextShow(t, false, textMaxWidth - imageLabel->width() - 80, this));
        }
        thinkButton = new ThinkingButton(this);
        thinkButton->setIsShowThinkContent(thinkIsExpand);
        connect(thinkButton, &ThinkingButton::clicked, this, &MessageWidget::thinkButtonClicked);
        textLayout->addWidget(thinkButton);
        for (int i = 0; i < thinkTextShowList.size(); ++i) {
            thinkBackVLayout->addWidget(thinkTextShowList[i]);
            if (i < thinkCodeShowList.size())
                thinkBackVLayout->addWidget(thinkCodeShowList[i]);
        }
        textLayout->addWidget(thinkBackWidget);
        thinkBackWidget->setVisible(thinkIsExpand);
        if (thinkTextIsRecvEnd && isRecvFirst) {
            thinkButton->setThinkEnd();
            isRecvFirst = false;
        }
    }

    if (!result.isEmpty()) {
        auto blocks = extractCodeBlocks(result);
        QString temp = result;
        QStringList splitTexts;
        for (const auto &b : blocks) {
            int idx = temp.indexOf(QString("```%1\n").arg(b.language));
            splitTexts.append(temp.left(idx));
            temp = temp.mid(idx + QString("```%1\n").arg(b.language).size() + b.code.size() + 3);
            auto *code =
                    new CodeShow(b.code, b.language, textMaxWidth - imageLabel->width() - 35, this);
            code->connectCodeCopyButtonClick(copyFun);
            resultCodeShowList.append(code);
        }
        splitTexts.append(temp);
        for (const auto &t : splitTexts) {
            if (!t.isEmpty())
                resultTextShowList.append(
                        new TextShow(t, false, textMaxWidth - imageLabel->width() - 35, this));
        }
        int j = 0;
        for (int i = 0; i < resultCodeShowList.size(); ++i) {
            if (i < splitTexts.size() && !splitTexts[i].isEmpty())
                textLayout->addWidget(resultTextShowList[i - j]);
            else
                ++j;
            textLayout->addWidget(resultCodeShowList[i]);
        }
        if (!splitTexts.isEmpty() && !splitTexts.last().isEmpty())
            textLayout->addWidget(resultTextShowList.last());
    }

    loadingWidget = new LoadingWidget(this);
    textBoxLayout->addWidget(textWidget);
    textBoxLayout->addWidget(loadingWidget);
    textBoxWidget->setFixedSize(qMax(textWidget->width(), loadingWidget->width()),
                                textWidget->height() + loadingWidget->height());
    loadingWidgetIsRemove = false;
}

QList<MessageWidget::CodeBlock> MessageWidget::extractCodeBlocks(const QString &text)
{
    QList<CodeBlock> list;
    QRegularExpression re("```(\\w+)\\n(.*?)```", QRegularExpression::DotMatchesEverythingOption);
    auto it = re.globalMatch(text);
    while (it.hasNext()) {
        auto m = it.next();
        QString lang = m.captured(1);
        QString code = m.captured(2);
        if (lang == "cpp" || lang == "python" || lang == "glsl" || lang == "lua")
            list.append({ lang, code, true });
    }
    return list;
}

void MessageWidget::parseThinkAndResult(const QString &txt, QString &think, QString &result,
                                        bool &thinkEnd)
{
    thinkEnd = false;
    if (txt.contains("<think>")) {
        int p1 = txt.indexOf("<think>") + 7;
        int p2 = txt.indexOf("</think>", p1);
        if (p2 > 0) {
            think = txt.mid(p1, p2 - p1);
            result = txt.mid(p2 + 8);
            thinkEnd = true;
        } else {
            think = txt.mid(p1);
        }
    } else if (!txt.contains("<think>") && !txt.isEmpty()) {
        result = txt;
        thinkEnd = true;
    }
}

bool MessageWidget::getIsUser()
{
    return isUser;
}

QString MessageWidget::getText()
{
    return text;
}

void MessageWidget::updateFunWidgetSize(qreal curDpi, qreal initDpi)
{
    Q_UNUSED(curDpi);
    Q_UNUSED(initDpi);
}

void MessageWidget::toggleWidget()
{
    connect(textShow, &TextShow::setSizeFinished, this, &MessageWidget::onSizeFinshed);
    textShow->toggleWidget();
}

void MessageWidget::breakHandle() { }

void MessageWidget::removeRenewResponseButton() { }

void MessageWidget::setSize()
{
    setFixedSize(textShow->size() + QSize(10, 0));
}

void MessageWidget::setText(const QString &text)
{
    this->text = text;
    textShow->setText(text);
    setFixedSize(textShow->size() + QSize(10, 0));
}

void MessageWidget::removeLoadingWidget() { }

void MessageWidget::onSizeFinshed()
{
    emit resizeFinished();
    if (!isUser)
        emit setTexting(false);
}

ListWidget *MessageWidget::getListWidget()
{
    return listWidget;
}

bool MessageWidget::hasSelectedText()
{
    return false;
}

QString MessageWidget::getSelectedText()
{
    return "";
}
