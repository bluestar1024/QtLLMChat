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
    funHLayout = new QHBoxLayout(funWidget);
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
    auto *textShow = new TextShow(text, textMaxWidth - imageLabel->width() - 15);
    textLayout->addWidget(textShow);
    textWidget->setFixedSize(textShow->width() + 10, textShow->height());
    textBoxLayout->addWidget(textWidget);
    textBoxLayout->addWidget(funWidget);
    textBoxWidget->setFixedSize(qMax(textWidget->width(), funWidget->width()),
                                textWidget->height() + funWidget->height());
}

void MessageWidget::buildAiUi()
{
    parseThinkAndResult(text, thinkText, resultText, thinkTextIsRecvEnd);

    thinkBackWidget = new ThinkBackWidget(this);
    thinkBackVLayout = new QVBoxLayout(thinkBackWidget);
    thinkBackVLayout->setContentsMargins(30, 0, 15, 0);
    thinkBackVLayout->setSpacing(0);

    if (!thinkText.isEmpty() && !QString("</think>").contains(thinkText)) {
        auto thinkCodeBlocks = extractCodeBlocks(thinkText);
        QStringList thinkSplitTextList;
        QString thinkTempText = thinkText;
        for (const auto &codeBlock : thinkCodeBlocks) {
            int idx = thinkTempText.indexOf(QString("```%1\n").arg(codeBlock.language));
            thinkSplitTextList.append(thinkTempText.left(idx));
            if (!codeBlock.endMarker.isEmpty())
                thinkTempText =
                        thinkTempText.mid(idx + QString("```%1\n").arg(codeBlock.language).size()
                                          + codeBlock.code.size() + 3);
            else
                thinkTempText =
                        thinkTempText.mid(idx + QString("```%1\n").arg(codeBlock.language).size()
                                          + codeBlock.code.size());
            auto *codeShow = new CodeShow(codeBlock.code, codeBlock.language,
                                          textMaxWidth - imageLabel->width() - 80, this);
            codeShow->connectCodeCopyButtonClick(this, copyFun);
            thinkCodeShowList.append(codeShow);
        }
        thinkSplitTextList.append(thinkTempText);
        for (const auto &splitText : thinkSplitTextList) {
            if (!splitText.isEmpty())
                thinkTextShowList.append(
                        new ThinkWidget(splitText, textMaxWidth - imageLabel->width() - 80, this));
        }
        thinkButton = new ThinkingButton();
        thinkButton->setIsShowThinkContent(thinkIsExpand);
        thinkButton->connectButtonClick(this, &MessageWidget::thinkButtonClicked);
        thinkButtonHaveCreated = true;
        textLayout->addWidget(thinkButton);
        int j = 0;
        for (int i = 0; i < thinkCodeShowList.size(); ++i) {
            if (!thinkSplitTextList[i].isEmpty())
                thinkBackVLayout->addWidget(thinkTextShowList[i - j]);
            else
                j += 1;
            thinkBackVLayout->addWidget(thinkCodeShowList[i]);
        }
        if (!thinkSplitTextList.last().isEmpty())
            thinkBackVLayout->addWidget(thinkTextShowList.last());
        textLayout->addWidget(thinkBackWidget);
        thinkBackWidget->setVisible(thinkIsExpand);
        if (thinkTextIsRecvEnd && isRecvFirst) {
            thinkButton->setThinkEnd();
            isRecvFirst = false;
        }
    }

    if (!resultText.isEmpty()) {
        auto resultCodeBlocks = extractCodeBlocks(resultText);
        QStringList resultSplitTextList;
        QString resultTempText = resultText;
        for (const auto &codeBlock : resultCodeBlocks) {
            int idx = resultTempText.indexOf(QString("```%1\n").arg(codeBlock.language));
            resultSplitTextList.append(resultTempText.left(idx));
            if (!codeBlock.endMarker.isEmpty())
                resultTempText =
                        resultTempText.mid(idx + QString("```%1\n").arg(codeBlock.language).size()
                                           + codeBlock.code.size() + 3);
            else
                resultTempText =
                        resultTempText.mid(idx + QString("```%1\n").arg(codeBlock.language).size()
                                           + codeBlock.code.size());
            auto *codeShow = new CodeShow(codeBlock.code, codeBlock.language,
                                          textMaxWidth - imageLabel->width() - 35, this);
            codeShow->connectCodeCopyButtonClick(this, copyFun);
            resultCodeShowList.append(codeShow);
        }
        resultSplitTextList.append(resultTempText);
        for (const auto &splitText : resultSplitTextList) {
            if (!splitText.isEmpty())
                resultTextShowList.append(
                        new TextShow(splitText, textMaxWidth - imageLabel->width() - 35, this));
        }
        int j = 0;
        for (int i = 0; i < resultCodeShowList.size(); ++i) {
            if (!resultSplitTextList[i].isEmpty())
                textLayout->addWidget(resultTextShowList[i - j]);
            else
                j += 1;
            textLayout->addWidget(resultCodeShowList[i]);
        }
        if (!resultSplitTextList.last().isEmpty())
            textLayout->addWidget(resultTextShowList.last());
    }

    adjustAiTextWidgetSize();
    loadingWidget = new LoadingWidget();
    textBoxLayout->addWidget(textWidget);
    textBoxLayout->addWidget(loadingWidget);
    textBoxWidget->setFixedSize(qMax(textWidget->width(), loadingWidget->width()),
                                textWidget->height() + loadingWidget->height());
    loadingWidgetIsRemove = false;
}

QList<MessageWidget::CodeBlock> MessageWidget::extractCodeBlocks(const QString &text)
{
    QList<CodeBlock> list;
    QRegularExpression re("```(\\w+)\\n(.*?)(```|$)",
                          QRegularExpression::DotMatchesEverythingOption);
    auto matchIterator = re.globalMatch(text);
    while (matchIterator.hasNext()) {
        auto match = matchIterator.next();
        QString lang = match.captured(1);
        QString code = match.captured(2);
        QString endMarker = match.captured(3);
        if (lang == "cpp" || lang == "python" || lang == "glsl" || lang == "lua")
            list.append({ lang, code, endMarker });
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
    } else if (!QString("<think>").contains(txt) && !txt.isEmpty()) {
        result = txt;
        thinkEnd = true;
    }
}

void MessageWidget::setSize()
{
    if (isUser) {
        if (!textShow)
            return;
        textWidget->setFixedSize(textShow->width() + 10, textShow->height());
        textBoxWidget->setFixedSize(qMax(textWidget->width(), funWidget->width()),
                                    textWidget->height() + funWidget->height());
    } else {
        adjustAiTextWidgetSize();
        if (loadingWidgetIsRemove)
            textBoxWidget->setFixedSize(qMax(textWidget->width(), funWidget->width()),
                                        textWidget->height() + funWidget->height());
        else
            textBoxWidget->setFixedSize(qMax(textWidget->width(), loadingWidget->width()),
                                        textWidget->height() + loadingWidget->height());
    }
    setFixedSize(imageLabel->width() + textBoxWidget->width() + 5,
                 qMax(imageLabel->height(), textBoxWidget->height()));
}

void MessageWidget::adjustAiTextWidgetSize()
{
    int thinkBackWidth = 0, thinkBackHeight = 0;
    for (auto *textShow : thinkTextShowList) {
        thinkBackWidth = qMax(thinkBackWidth, textShow->width());
        thinkBackHeight += textShow->height();
    }
    for (auto *codeShow : thinkCodeShowList) {
        thinkBackWidth = qMax(thinkBackWidth, codeShow->width());
        thinkBackHeight += codeShow->height();
    }
    if (!thinkTextShowList.isEmpty() || !thinkCodeShowList.isEmpty()) {
        thinkBackWidth += 45;
        thinkBackWidget->setFixedSize(thinkBackWidth, thinkBackHeight);
    }

    int thinkWidth = 0, thinkHeight = 0;
    int resultWidth = 0, resultHeight = 0;
    bool hasThink = !thinkText.isEmpty() && !QString("</think>").contains(thinkText);
    if (hasThink) {
        if (thinkIsExpand) {
            thinkWidth = qMax(thinkButton->width(), thinkBackWidget->width());
            thinkHeight = thinkButton->height() + thinkBackWidget->height();
        } else {
            thinkWidth = thinkButton->width();
            thinkHeight = thinkButton->height();
        }
    }
    if (!resultText.isEmpty()) {
        for (auto *textShow : resultTextShowList) {
            resultWidth = qMax(resultWidth, textShow->width());
            resultHeight += textShow->height();
        }
        for (auto *codeShow : resultCodeShowList) {
            resultWidth = qMax(resultWidth, codeShow->width());
            resultHeight += codeShow->height();
        }
    }

    textWidget->setFixedSize(qMax(thinkWidth, resultWidth) + 30, thinkHeight + resultHeight + 10);
}

bool MessageWidget::getThinkIsExpanded()
{
    if (!isUser)
        return thinkIsExpand;
    else
        return false;
}

void MessageWidget::thinkButtonClicked()
{
    thinkIsExpand = !thinkIsExpand;
    thinkBackWidget->setVisible(thinkIsExpand);
    emit resizeFinished();
}

void MessageWidget::onAiUpdateSize()
{
    emit resizeFinished();
    emit setTexting(false);
    if (!thinkText.isEmpty() && !QString('</think>').contains(thinkText)) {
        if (thinkButton->getThinkTimeLength() == 0)
            thinkButton->setThinkTimeLength(thinkTimeLengthList[thinkTimeIndex]);
        else
            thinkTimeLengthList[thinkTimeIndex] = thinkButton->getThinkTimeLength();
    }
}

void MessageWidget::showFunWidget()
{
    if (funWidgetIsShow)
        return;
    copyButton->show();
    if (!renewButtonIsRemove)
        renewButton->show();
    funWidgetIsShow = true;
}

void MessageWidget::hideFunWidget()
{
    if (!funWidgetIsShow)
        return;
    copyButton->hide();
    if (!renewButtonIsRemove)
        renewButton->hide();
    funWidgetIsShow = false;
}

bool MessageWidget::getIsUser()
{
    return isUser;
}

QString MessageWidget::getText()
{
    return text;
}

TextWidget *MessageWidget::getTextWidget()
{
    return textWidget;
}

TextBoxWidget *MessageWidget::getTextBoxWidget()
{
    return textBoxWidget;
}

void MessageWidget::updateFunWidgetSize(qreal curDpi, qreal initDpi)
{
    if (initDpi <= 0)
        return;
    const qreal ratio = qreal(curDpi) / initDpi;
    auto scale = [=](int v) { return qRound(v * ratio); };

    copyButton->setFixedSize(scale(copyButton->width()), scale(copyButton->height()));
    QMargins m = funHLayout->contentsMargins();
    funHLayout->setContentsMargins(scale(m.left()), scale(m.top()), scale(m.right()),
                                   scale(m.bottom()));
    if (isUser) {
        funWidget->setFixedSize(scale(funWidget->width()), scale(funWidget->height()));
        textBoxWidget->setFixedSize(qMax(textWidget->width(), funWidget->width()),
                                    textWidget->height() + funWidget->height());
    } else {
        if (!renewResponseButtonIsRemove) {
            renewResponseButton->setFixedSize(scale(renewResponseButton->width()),
                                              scale(renewResponseButton->height()));
            funHLayout->setSpacing(scale(funHLayout->spacing()));
        }
        funWidget->setFixedSize(scale(funWidget->width()), scale(funWidget->height()));
        if (!loadingWidgetIsRemove)
            return;
        textBoxWidget->setFixedSize(qMax(textWidget->width(), funWidget->width()),
                                    textWidget->height() + funWidget->height());
    }
    setFixedSize(imageLabel->width() + textBoxWidget->width() + 5,
                 qMax(imageLabel->height(), textBoxWidget->height()));
}

void MessageWidget::toggleWidget()
{
    if (isUser) {
        if (textShow) {
            connect(textShow, &TextShow::setSizeFinished, this, &MessageWidget::onSizeFinished);
            textShow->toggleWidget();
        }
    } else {
        for (auto *thinkWidget : thinkTextShowList) {
            connect(thinkWidget, &TextShow::setSizeFinished, this, &MessageWidget::onSizeFinished);
            thinkWidget->toggleWidget();
        }
        for (auto *textShow : resultTextShowList) {
            connect(textShow, &TextShow::setSizeFinished, this, &MessageWidget::onSizeFinished);
            textShow->toggleWidget();
        }
    }
}

void MessageWidget::breakHandle()
{
    if (!thinkTextIsRecvEnd) {
        thinkButton->setThinkEnd();
        thinkTimeLengthList[thinkTimeIndex] = thinkButton->getThinkTimeLength();
    }
}

void MessageWidget::removeRenewResponseButton() { }

void MessageWidget::setText(const QString &text)
{
    this->text = text;
    if (isUser) {
        if (textShow)
            textShow->setText(this->text);
        setSize();
        return;
    }

    emit setTexting(true);

    parseThinkAndResult(this->text, thinkText, resultText, thinkTextIsRecvEnd);

    if (!thinkText.isEmpty() && !QString("</think>").contains(thinkText)) {
        auto thinkCodeBlocks = extractCodeBlocks(thinkText);
        QStringList thinkSplitTextList;
        QString thinkTempText = thinkText;
        int thinkCodeShowListLastLen = thinkCodeShowList.size() - 1;
        for (int i = 0; i < thinkCodeBlocks.size(); i++) {
            int idx = thinkTempText.indexOf(QString("```%1\n").arg(thinkCodeBlocks[i].language));
            thinkSplitTextList.append(thinkTempText.left(idx));
            if (!thinkCodeBlocks[i].endMarker.isEmpty()) {
                thinkTempText = thinkTempText.mid(
                        idx + QString("```%1\n").arg(thinkCodeBlocks[i].language).size()
                        + thinkCodeBlocks[i].code.size() + 3);
            } else {
                thinkTempText = thinkTempText.mid(
                        idx + QString("```%1\n").arg(thinkCodeBlocks[i].language).size()
                        + thinkCodeBlocks[i].code.size());
            }
            if (thinkCodeShowListLastLen < i) {
                auto *codeShow = new CodeShow(thinkCodeBlocks[i].code, thinkCodeBlocks[i].language,
                                              textMaxWidth - imageLabel->width() - 80, this);
                codeShow->connectCodeCopyButtonClick(this, copyFun);
                codeShow->setVisible(thinkIsExpand);
                thinkCodeShowList.append(codeShow);
            } else {
                thinkCodeShowList[i]->setText(thinkCodeBlocks[i].code, thinkCodeBlocks[i].language);
            }
        }
        thinkSplitTextList.append(thinkTempText);
        if (!thinkButtonHaveCreated) {
            thinkButton = new ThinkingButton();
            thinkButton->setIsShowThinkContent(thinkIsExpand);
            thinkButton->connectButtonClick(this, &MessageWidget::thinkButtonClicked);
            thinkButtonHaveCreated = true;
            textLayout->addWidget(thinkButton);
            textLayout->addWidget(thinkBackWidget);
            thinkBackWidget->setVisible(thinkIsExpand);
        }
        int i = 0;
        int thinkTextShowListLastLen = thinkTextShowList.size() - 1;
        for (const auto &splitText : thinkSplitTextList) {
            if (!splitText.isEmpty()) {
                if (thinkTextShowListLastLen < i) {
                    ThinkWidget *thinkWidget = new ThinkWidget(
                            splitText, textMaxWidth - imageLabel->width() - 80, this);
                    thinkWidget.setVisible(thinkIsExpand);
                    thinkTextShowList.append(thinkWidget);
                } else {
                    thinkTextShowList[i]->setText(splitText);
                }
                i += 1;
            }
        }
        int j = 0;
        for (int i = 0; i < thinkCodeShowList.size(); ++i) {
            if (!thinkSplitTextList[i].isEmpty()) {
                if (thinkTextShowListLastLen < i - j)
                    thinkBackVLayout->addWidget(thinkTextShowList[i - j]);
            } else {
                j += 1;
            }
            if (thinkCodeShowListLastLen < i)
                thinkBackVLayout->addWidget(thinkCodeShowList[i]);
        }
        if (thinkTextShowListLastLen < thinkTextShowList.size() - 1 - j
            && !thinkSplitTextList.last().isEmpty())
            thinkBackVLayout->addWidget(thinkTextShowList.last());
        if (thinkTextIsRecvEnd && isRecvFirst) {
            thinkButton->setThinkEnd();
            isRecvFirst = false;
        }
    }

    if (!resultText.isEmpty()) {
        auto resultCodeBlocks = extractCodeBlocks(resultText);
        QStringList resultSplitTextList;
        QString resultTempText = resultText;
        int resultCodeShowListLastLen = resultCodeShowList.size() - 1;
        for (int i = 0; i < resultCodeBlocks.size(); i++) {
            int idx = resultTempText.indexOf(QString("```%1\n").arg(resultCodeBlocks[i].language));
            resultSplitTextList.append(resultTempText.left(idx));
            resultTempText = resultTempText.mid(
                    idx + QString("```%1\n").arg(resultCodeBlocks[i].language).size()
                    + resultCodeBlocks[i].code.size() + 3);
            if (resultCodeShowListLastLen < i) {
                auto *codeShow =
                        new CodeShow(resultCodeBlocks[i].code, resultCodeBlocks[i].language,
                                     textMaxWidth - imageLabel->width() - 35, this);
                codeShow->connectCodeCopyButtonClick(this, copyFun);
                resultCodeShowList.append(codeShow);
            } else {
                resultCodeShowList[i]->setText(resultCodeBlocks[i].code,
                                               resultCodeBlocks[i].language);
            }
        }
        resultSplitTextList.append(resultTempText);
        int i = 0;
        int resultTextShowListLastLen = resultTextShowList.size() - 1;
        for (const auto &splitText : resultSplitTextList) {
            if (!splitText.isEmpty()) {
                if (resultTextShowListLastLen < i)
                    resultTextShowList.append(
                            TextShow(splitText, textMaxWidth - imageLabel->width() - 35, this));
                else
                    resultTextShowList[i]->setText(splitText);
                i += 1;
            }
        }
        int j = 0;
        for (int i = 0; i < resultCodeShowList.size(); ++i) {
            if (!resultSplitTextList[i].isEmpty()) {
                if (resultTextShowListLastLen < i - j)
                    textLayout->addWidget(resultTextShowList[i - j]);
            } else {
                j += 1;
            }
            if (resultCodeShowListLastLen < i)
                textLayout->addWidget(resultCodeShowList[i]);
        }
        if (resultTextShowListLastLen < resultTextShowList.size() - 1 - j
            && !resultSplitTextList.last().isEmpty())
            textLayout->addWidget(resultTextShowList.last());
    }

    setSize();
    emit setTexting(false);
}

void MessageWidget::removeLoadingWidget()
{
    if (isUser || loadingWidgetIsRemove)
        return;
    textBoxLayout->removeWidget(loadingWidget);
    loadingWidget->deleteLater();
    loadingWidgetIsRemove = true;
    textBoxLayout->addWidget(funWidget);
    textBoxWidget->setFixedSize(qMax(textWidget->width(), funWidget->width()),
                                textWidget->height() + funWidget->height());
    setFixedSize(imageLabel->width() + textBoxWidget->width() + 5,
                 qMax(imageLabel->height(), textBoxWidget->height()));
}

void MessageWidget::onSizeFinished()
{
    if (isUser)
        emit resizeFinished();
    else
        aiUpdateSizeTimer.start(10);
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
