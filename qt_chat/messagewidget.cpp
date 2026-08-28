#include "messagewidget.h"
#include "appcontext.h"

#include <QtCore/QMutexLocker>
#include <QtCore/QPointer>

MessageWidget::MessageWidget(AppContext *appContext, const QString &text,
                             std::function<void()> copyFun, std::function<void()> renewResponseFun,
                             std::function<void(MessageWidget *)> widgetResizeFun,
                             std::function<void(bool)> getSetTextingFun,
                             std::function<void()> executeNextFun, ListWidget *listWidget,
                             QList<int> &thinkTimeLengthList, int thinkTimeIndex, bool isUser,
                             bool thinkIsExpand, int textMaxWidth, QWidget *parent)
    : QWidget(parent),
      appContext(appContext),
      text(text),
      copyFun(copyFun),
      renewResponseFun(renewResponseFun),
      widgetResizeFun(widgetResizeFun),
      getSetTextingFun(getSetTextingFun),
      executeNextFun(executeNextFun),
      listWidget(listWidget),
      thinkTimeLengthList(thinkTimeLengthList),
      thinkTimeIndex(thinkTimeIndex),
      isUser(isUser),
      thinkIsExpand(thinkIsExpand),
      textMaxWidth(textMaxWidth - 10),
      thinkButtonHaveCreated(false),
      thinkText(""),
      resultText(""),
      thinkTextIsRecvEnd(false),
      isRecvFirst(true),
      funWidgetIsShow(false),
      loadingWidgetIsRemove(true),
      renewResponseButtonIsRemove(true)
{
    setMouseTracking(true);

    connect(this, &MessageWidget::resizeFinished, this->widgetResizeFun);
    connect(this, &MessageWidget::setTexting, this->getSetTextingFun);

    imageLabel = new ImageLabel(this->appContext, isUser);
    textWidget = new TextWidget(isUser);
    textLayout = new QVBoxLayout(textWidget);
    textLayout->setSpacing(0);
    if (this->isUser)
        textLayout->setContentsMargins(5, 0, 5, 0);
    else
        textLayout->setContentsMargins(15, 5, 15, 5);

    textBoxWidget = new TextBoxWidget();
    textBoxLayout = new QVBoxLayout(textBoxWidget);
    textBoxLayout->setSpacing(0);
    textBoxLayout->setContentsMargins(0, 0, 0, 0);

    funWidget = new QWidget();
    qDebug() << "MessageWidget funWidget:" << funWidget;
    funHLayout = new QHBoxLayout(funWidget);
    funHLayout->setContentsMargins(5, 5, 5, 5);
    copyButton = new CopyButton(this->appContext, "复制", 15, 35);
    qDebug() << "this:" << this << "qobject_cast<MessageWidget *>(copyButton->parent()):"
             << qobject_cast<MessageWidget *>(copyButton->parent());
    copyButton->setFixedSize(16, 16);
    copyImagesPath = this->appContext->imagesDir() + "/copy.png";
    copyHoverImagesPath = this->appContext->imagesDir() + "/copy_hover.png";
    copyButton->setStyleSheet(QString("QPushButton{ border-image:url(%1); }"
                                      "QPushButton:hover{ border-image:url(%2); }")
                                      .arg(copyImagesPath, copyHoverImagesPath));
    connect(copyButton, &QPushButton::clicked, this->copyFun);
    funHLayout->addWidget(copyButton);
    // qDebug() << "this:" << this
    //          << "qobject_cast<MessageWidget *>(copyButton->parent()):" << copyButton->parent();
    // copyButton->setParent(this);
    // qDebug() << "this:" << this << "qobject_cast<MessageWidget *>(copyButton->parent()):"
    //          << qobject_cast<MessageWidget *>(copyButton->parent());
    copyButton->hide();

    if (!isUser) {
        renewResponseButton = new PushButton(this->appContext, "重新生成响应", 25, 35);
        renewResponseButton->setFixedSize(16, 16);
        renewResponseImagesPath = this->appContext->imagesDir() + "/renewResponse.png";
        renewResponseHoverImagesPath = this->appContext->imagesDir() + "/renewResponse_hover.png";
        renewResponseButton->setStyleSheet(
                QString("QPushButton{ border-image:url(%1); }"
                        "QPushButton:hover{ border-image:url(%2); }")
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

    checkTimer.setInterval(2);

    // 构造渲染期间置流式状态，使嵌套事件循环内的窗口重建请求延迟执行，防止控件重复创建与悬空访问
    emit setTexting(true);
    if (isUser)
        buildUserUi();
    else
        buildAiUi();

    subVLayout1 = new QVBoxLayout();
    subVLayout1->setAlignment(Qt::AlignTop);
    subVLayout1->setContentsMargins(0, 0, 0, 0);
    subVLayout2 = new QVBoxLayout();
    subVLayout2->setAlignment(Qt::AlignTop);
    subVLayout2->setContentsMargins(0, 0, 0, 0);
    mainHLayout = new QHBoxLayout(this);
    mainHLayout->setContentsMargins(0, 0, 0, 0);
    mainHLayout->setSpacing(5);
    if (isUser) {
        // mainHLayout->addWidget(textBoxWidget);
        // mainHLayout->addWidget(imageLabel);
        subVLayout1->addWidget(textBoxWidget);
        subVLayout2->addWidget(imageLabel);
    } else {
        // mainHLayout->addWidget(imageLabel);
        // mainHLayout->addWidget(textBoxWidget);
        subVLayout1->addWidget(imageLabel);
        subVLayout2->addWidget(textBoxWidget);
    }
    mainHLayout->addLayout(subVLayout1);
    mainHLayout->addLayout(subVLayout2);
    setFixedSize(imageLabel->width() + textBoxWidget->width() + 5,
                 qMax(imageLabel->height(), textBoxWidget->height()));
    // emit resizeFinished(this);
    // 构造渲染完成，若有待执行的重建，将在事件循环空闲时执行
    emit setTexting(false);
}

MessageWidget::~MessageWidget() { }

void MessageWidget::buildUserUi()
{
    // sizeFinishFun 捕获裸 this：控件销毁链（如 chatShow->clear()）中子控件析构可能
    // 触发信号回调到这里，用 QPointer 保护，对象已销毁时安全跳过
    QPointer<MessageWidget> self(this);
    textShow = new TextShow(
            appContext, text,
            [self]() {
                if (self)
                    self->onSizeFinished();
            },
            executeNextFun, textMaxWidth - imageLabel->width() - 15);
    // connect(textShow, &TextShow::executeNext, this->executeNextFun);
    // connect(textShow, &ThinkWidget::setSizeFinished, this, &MessageWidget::onSizeFinished);
    // if (textShow->getIsEmitSizeFinish()) {
    //     textShow->setIsEmitSizeFinish(false);
    //     emit textShow->setSizeFinished();
    // }
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
    qDebug() << "parseThinkAndResult:" << thinkText << "and" << resultText << this;
    thinkBackWidget = new ThinkBackWidget(this);
    thinkBackVLayout = new QVBoxLayout(thinkBackWidget);
    thinkBackVLayout->setContentsMargins(30, 0, 15, 0);
    thinkBackVLayout->setSpacing(0);

    if (!thinkText.isEmpty() && !QString("</think>").contains(thinkText)) {
        qDebug() << "new ThinkingButton()";
        thinkButton = new ThinkingButton(appContext);
        thinkButton->setIsShowThinkContent(thinkIsExpand);
        thinkButton->connectButtonClick(this, &MessageWidget::thinkButtonClicked);
        thinkButtonHaveCreated = true;
        textLayout->addWidget(thinkButton);

        auto thinkCodeBlocks = extractCodeBlocks(thinkText);
        QStringList thinkSplitTextList;
        QString thinkTempText = thinkText;
        qDebug() << "thinkText:" << thinkText << this;
        for (const auto &codeBlock : thinkCodeBlocks) {
            int idx = thinkTempText.indexOf(QString("```%1\n").arg(codeBlock.language));
            thinkSplitTextList.append(thinkTempText.left(idx));
            qDebug() << "codeBlock:" << codeBlock.language << codeBlock.code << codeBlock.endMarker;
            if (!codeBlock.endMarker.isEmpty())
                thinkTempText =
                        thinkTempText.mid(idx + QString("```%1\n").arg(codeBlock.language).size()
                                          + codeBlock.code.size() + 3);
            else
                thinkTempText =
                        thinkTempText.mid(idx + QString("```%1\n").arg(codeBlock.language).size()
                                          + codeBlock.code.size());
            qDebug() << "thinkTempText:" << thinkTempText;
            QPointer<MessageWidget> thinkCodeSelf(this);
            auto *codeShow = new CodeShow(
                    appContext, codeBlock.code, codeBlock.language,
                    [thinkCodeSelf]() {
                        if (thinkCodeSelf)
                            thinkCodeSelf->onSizeFinished();
                    },
                    textMaxWidth - imageLabel->width() - 80, this);
            qDebug() << "new CodeShow";
            codeShow->hide();
            qDebug() << "hide CodeShow";
            codeShow->connectCodeCopyButtonClick(copyFun);
            qDebug() << "connect CodeShow";
            thinkCodeShowList.append(codeShow);
            qDebug() << "append CodeShow";
        }
        thinkSplitTextList.append(thinkTempText);
        qDebug() << "thinkSplitTextList:" << thinkSplitTextList;
        for (const auto &splitText : thinkSplitTextList) {
            if (!splitText.trimmed().isEmpty()) {
                QPointer<MessageWidget> thinkTextSelf(this);
                thinkTextShowList.append(new ThinkWidget(
                        appContext, splitText,
                        [thinkTextSelf]() {
                            if (thinkTextSelf)
                                thinkTextSelf->onSizeFinished();
                        },
                        textMaxWidth - imageLabel->width() - 80, this));
                // connect(thinkTextShowList.last(), &ThinkWidget::setSizeFinished, this,
                //         &MessageWidget::onSizeFinished);
                // if (thinkTextShowList.last()->getIsEmitSizeFinish()) {
                //     thinkTextShowList.last()->setIsEmitSizeFinish(false);
                //     emit thinkTextShowList.last()->setSizeFinished();
                // }
            }
        }
        int j = 0;
        for (int i = 0; i < thinkCodeShowList.size(); ++i) {
            if (!thinkSplitTextList[i].trimmed().isEmpty()) {
                thinkBackVLayout->addWidget(thinkTextShowList[i - j]);
                // 防重入：流式追加期间（重建后的全量刷新等）嵌套事件循环会递归进入本等待块，
                // 同一 QEventLoop 实例重复 exec() 打印警告并立即返回 -1，且 [&] 捕获的
                // eventLoopQuitConnect 悬空导致崩溃；重入时跳过等待，渲染完成由
                // sizeFinishFun -> onSizeFinished 兜底触发布局更新
                if (!loop.isRunning()) {
                    QMetaObject::Connection eventLoopQuitConnect =
                            QObject::connect(&checkTimer, &QTimer::timeout, [&]() {
                                if (thinkTextShowList[i - j]->getIsSizeFinish()) {
                                    thinkTextShowList[i - j]->setIsSizeFinish(false);
                                    bool ok = QObject::disconnect(eventLoopQuitConnect);
                                    qDebug() << "connect-1" << ok;
                                    checkTimer.stop();
                                    loop.quit();
                                    qDebug() << "loop quit-1";
                                }
                            });
                    checkTimer.start();
                    qDebug() << "checkTimer start-1";
                    loop.exec();
                    qDebug() << "loop exec-1";
                }
            } else
                j += 1;
            thinkBackVLayout->addWidget(thinkCodeShowList[i]);
            thinkCodeShowList[i]->show();
            onSizeFinished();
        }
        if (!thinkSplitTextList.last().isEmpty()) {
            thinkBackVLayout->addWidget(thinkTextShowList.last());
            // 防重入：同一 QEventLoop 实例嵌套 exec() 会返回 -1 且产生悬空 lambda，重入时跳过等待
            if (!loop.isRunning()) {
                QMetaObject::Connection eventLoopQuitConnect =
                        QObject::connect(&checkTimer, &QTimer::timeout, [&]() {
                            if (thinkTextShowList.last()->getIsSizeFinish()) {
                                thinkTextShowList.last()->setIsSizeFinish(false);
                                bool ok = QObject::disconnect(eventLoopQuitConnect);
                                qDebug() << "disconnect0" << ok;
                                checkTimer.stop();
                                loop.quit();
                                qDebug() << "loop quit0";
                            }
                        });
                checkTimer.start();
                qDebug() << "checkTimer start0";
                loop.exec();
                qDebug() << "loop exec0";
            }
        }
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
        qDebug() << "resultText:" << resultText << this;
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
            QPointer<MessageWidget> resultCodeSelf(this);
            auto *codeShow = new CodeShow(
                    appContext, codeBlock.code, codeBlock.language,
                    [resultCodeSelf]() {
                        if (resultCodeSelf)
                            resultCodeSelf->onSizeFinished();
                    },
                    textMaxWidth - imageLabel->width() - 35, this);
            codeShow->hide();
            codeShow->connectCodeCopyButtonClick(copyFun);
            resultCodeShowList.append(codeShow);
        }
        resultSplitTextList.append(resultTempText);
        for (const auto &splitText : resultSplitTextList) {
            if (!splitText.trimmed().isEmpty()) {
                QPointer<MessageWidget> resultTextSelf(this);
                resultTextShowList.append(new TextShow(
                        appContext, splitText,
                        [resultTextSelf]() {
                            if (resultTextSelf)
                                resultTextSelf->onSizeFinished();
                        },
                        nullptr, textMaxWidth - imageLabel->width() - 35, this));
                // connect(resultTextShowList.last(), &ThinkWidget::setSizeFinished, this,
                //         &MessageWidget::onSizeFinished);
                // if (resultTextShowList.last()->getIsEmitSizeFinish()) {
                //     resultTextShowList.last()->setIsEmitSizeFinish(false);
                //     emit resultTextShowList.last()->setSizeFinished();
                // }
            }
        }
        int j = 0;
        for (int i = 0; i < resultCodeShowList.size(); ++i) {
            if (!resultSplitTextList[i].trimmed().isEmpty()) {
                textLayout->addWidget(resultTextShowList[i - j]);
                // 防重入：同一 QEventLoop 实例嵌套 exec() 会返回 -1 且产生悬空 lambda，重入时跳过等待
                if (!loop.isRunning()) {
                    QMetaObject::Connection eventLoopQuitConnect =
                            QObject::connect(&checkTimer, &QTimer::timeout, [&]() {
                                if (resultTextShowList[i - j]->getIsSizeFinish()) {
                                    resultTextShowList[i - j]->setIsSizeFinish(false);
                                    bool ok = QObject::disconnect(eventLoopQuitConnect);
                                    qDebug() << "disconnect-1" << ok;
                                    checkTimer.stop();
                                    loop.quit();
                                    qDebug() << "loop quit-1";
                                }
                            });
                    checkTimer.start();
                    qDebug() << "checkTimer start-1";
                    loop.exec();
                    qDebug() << "loop exec-1";
                }
            } else
                j += 1;
            textLayout->addWidget(resultCodeShowList[i]);
            resultCodeShowList[i]->show();
            onSizeFinished();
        }
        if (!resultSplitTextList.last().isEmpty()) {
            textLayout->addWidget(resultTextShowList.last());
            // 防重入：同一 QEventLoop 实例嵌套 exec() 会返回 -1 且产生悬空 lambda，重入时跳过等待
            if (!loop.isRunning()) {
                QMetaObject::Connection eventLoopQuitConnect =
                        QObject::connect(&checkTimer, &QTimer::timeout, [&]() {
                            if (resultTextShowList.last()->getIsSizeFinish()) {
                                resultTextShowList.last()->setIsSizeFinish(false);
                                bool ok = QObject::disconnect(eventLoopQuitConnect);
                                qDebug() << "disconnect0" << ok;
                                checkTimer.stop();
                                loop.quit();
                                qDebug() << "loop quit0";
                            }
                        });
                checkTimer.start();
                qDebug() << "checkTimer start0";
                loop.exec();
                qDebug() << "loop exec0";
            }
        }
    }

    adjustAiTextWidgetSize();
    loadingWidget = new LoadingWidget();
    textBoxLayout->addWidget(textWidget);
    textBoxLayout->addWidget(loadingWidget);
    textBoxWidget->setFixedSize(qMax(textWidget->width(), loadingWidget->width()),
                                textWidget->height() + loadingWidget->height());
    loadingWidgetIsRemove = false;
    qDebug() << "buildAiUi" << thinkText << resultText;
}

QList<MessageWidget::CodeBlock> MessageWidget::extractCodeBlocks(const QString &text)
{
    QList<CodeBlock> list;
    QRegularExpression re("```(\\w+)\\n(.*?)(```|$)",
                          QRegularExpression::DotMatchesEverythingOption);
    // QRegularExpression re("```(\\w+)\\n(.*)(?=(```|$))(\\3)?",
    //                       QRegularExpression::DotMatchesEverythingOption);
    // QRegularExpression re("```(\\w+)\\n([\\s\\S]*?)(?=```|$)(\\3)?");
    // QRegularExpression re("```(\\w+)\\n(.*?)(```)?",
    //                       QRegularExpression::DotMatchesEverythingOption);
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
    qDebug() << "MessageWidget setSize start" << this;
    if (isUser) {
        qDebug() << "MessageWidget setSize ing0" << this;
        if (!textShow)
            return;
        textWidget->setFixedSize(textShow->width() + 10, textShow->height());
        textBoxWidget->setFixedSize(qMax(textWidget->width(), funWidget->width()),
                                    textWidget->height() + funWidget->height());
    } else {
        qDebug() << "MessageWidget setSize ing1" << this;
        adjustAiTextWidgetSize();
        if (loadingWidgetIsRemove)
            textBoxWidget->setFixedSize(qMax(textWidget->width(), funWidget->width()),
                                        textWidget->height() + funWidget->height());
        else
            textBoxWidget->setFixedSize(qMax(textWidget->width(), loadingWidget->width()),
                                        textWidget->height() + loadingWidget->height());
        qDebug() << "textBoxWidget:" << textBoxWidget->width() << textBoxWidget->height();
    }
    qDebug() << "MessageWidget setSize ing2" << this;
    setFixedSize(imageLabel->width() + textBoxWidget->width() + 5,
                 qMax(imageLabel->height(), textBoxWidget->height()));
    qDebug() << "MessageWidget size:" << this->width() << this->height();
    qDebug() << "MessageWidget setSize end" << this;
}

void MessageWidget::adjustAiTextWidgetSize()
{
    int thinkBackWidth = 0, thinkBackHeight = 0;
    for (auto *thinkWidget : thinkTextShowList) {
        // for (auto *thinkWidget :
        //      thinkBackWidget->findChildren<ThinkWidget *>(Qt::FindDirectChildrenOnly)) {
        thinkBackWidth = qMax(thinkBackWidth, thinkWidget->width());
        thinkBackHeight += thinkWidget->height();
        qDebug() << "thinkTextShowList:" << thinkBackWidth << thinkBackHeight;
    }
    for (auto *codeShow : thinkCodeShowList) {
        // for (auto *codeShow : thinkBackWidget->findChildren<CodeShow *>(Qt::FindDirectChildrenOnly)) {
        thinkBackWidth = qMax(thinkBackWidth, codeShow->width());
        thinkBackHeight += codeShow->height();
        qDebug() << "thinkCodeShowList:" << thinkBackWidth << thinkBackHeight;
    }
    if (!thinkTextShowList.isEmpty() || !thinkCodeShowList.isEmpty()) {
        thinkBackWidth += 45;
        thinkBackWidget->setFixedSize(thinkBackWidth, thinkBackHeight);
        qDebug() << "!List.isEmpty:" << thinkBackWidth << thinkBackHeight;
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
        qDebug() << "hasThink:" << thinkWidth << thinkHeight;
    }
    if (!resultText.isEmpty()) {
        for (auto *textShow : resultTextShowList) {
            // for (auto *textShow : textWidget->findChildren<TextShow *>(Qt::FindDirectChildrenOnly)) {
            resultWidth = qMax(resultWidth, textShow->width());
            resultHeight += textShow->height();
            qDebug() << "resultTextShowList:" << resultWidth << resultHeight;
        }
        for (auto *codeShow : resultCodeShowList) {
            // for (auto *codeShow : textWidget->findChildren<CodeShow *>(Qt::FindDirectChildrenOnly)) {
            resultWidth = qMax(resultWidth, codeShow->width());
            resultHeight += codeShow->height();
            qDebug() << "resultCodeShowList:" << resultWidth << resultHeight;
        }
    }

    textWidget->setFixedSize(qMax(thinkWidth, resultWidth) + 30, thinkHeight + resultHeight + 10);
    qDebug() << "textWidget:" << textWidget->width() << textWidget->height();
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
    setSize();
    emit resizeFinished(this);
}

void MessageWidget::syncThinkTimeLength()
{
    qDebug() << "syncThinkTimeLength before";
    if (!thinkText.isEmpty() && !QString("</think>").contains(thinkText)) {
        qDebug() << "syncThinkTimeLength before1";
        // 窗口重建后本控件可能持有过期的 thinkTimeIndex，列表越界时忽略同步
        if (thinkTimeIndex < 0 || thinkTimeIndex >= thinkTimeLengthList.size()) {
            qDebug() << "syncThinkTimeLength index out of range" << thinkTimeIndex
                     << thinkTimeLengthList.size();
            return;
        }
        if (thinkButton->getThinkTimeLength() == 0)
            thinkButton->setThinkTimeLength(thinkTimeLengthList[thinkTimeIndex]);
        else
            thinkTimeLengthList[thinkTimeIndex] = thinkButton->getThinkTimeLength();
        qDebug() << "syncThinkTimeLength after1";
    }
    qDebug() << "syncThinkTimeLength after";
}

void MessageWidget::showFunWidget()
{
    if (funWidgetIsShow)
        return;
    if (!loadingWidgetIsRemove)
        return;
    // qDebug() << "showFunWidget copyButton before:" << funHLayout->indexOf(copyButton);
    // if (-1 == funHLayout->indexOf(copyButton))
    //     funHLayout->insertWidget(0, copyButton);
    copyButton->show();
    // qDebug() << "showFunWidget copyButton after:" << funHLayout->indexOf(copyButton);
    if (!renewResponseButtonIsRemove) {
        // qDebug() << "showFunWidget renewResponseButton before:" << funHLayout->indexOf(renewResponseButton);
        // if (-1 == funHLayout->indexOf(renewResponseButton))
        //     funHLayout->addWidget(renewResponseButton);
        renewResponseButton->show();
        // qDebug() << "showFunWidget renewResponseButton after:" << funHLayout->indexOf(renewResponseButton);
    }
    funWidgetIsShow = true;
    // qDebug() << "showFunWidget funWidget:" << textBoxLayout->indexOf(funWidget);
}

void MessageWidget::hideFunWidget()
{
    if (!funWidgetIsShow)
        return;
    copyButton->hide();
    if (!renewResponseButtonIsRemove)
        renewResponseButton->hide();
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

bool MessageWidget::getIsRemoveloadingWidget()
{
    return loadingWidgetIsRemove;
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

// void MessageWidget::toggleWidget()
// {
//     if (isUser) {
//         if (textShow) {
//             connect(textShow, &TextShow::setSizeFinished, this, &MessageWidget::onSizeFinished);
//             textShow->toggleWidget();
//         }
//     } else {
//         for (auto *thinkWidget : thinkTextShowList) {
//             connect(thinkWidget, &ThinkWidget::setSizeFinished, this,
//                     &MessageWidget::onSizeFinished);
//             thinkWidget->toggleWidget();
//         }
//         for (auto *textShow : resultTextShowList) {
//             connect(textShow, &ThinkWidget::setSizeFinished, this, &MessageWidget::onSizeFinished);
//             textShow->toggleWidget();
//         }
//     }
// }

void MessageWidget::breakHandle()
{
    if (!thinkTextIsRecvEnd) {
        thinkButton->setThinkEnd();
        // 窗口重建后本控件可能持有过期的 thinkTimeIndex，列表越界时忽略同步
        if (thinkTimeIndex >= 0 && thinkTimeIndex < thinkTimeLengthList.size())
            thinkTimeLengthList[thinkTimeIndex] = thinkButton->getThinkTimeLength();
    }
}

void MessageWidget::removeRenewResponseButton()
{
    if (isUser || renewResponseButtonIsRemove)
        return;
    funHLayout->removeWidget(renewResponseButton);
    renewResponseButton->deleteLater();
    renewResponseButtonIsRemove = true;
    int w = copyButton->width() + funHLayout->contentsMargins().left()
            + funHLayout->contentsMargins().right();
    int h = copyButton->height() + funHLayout->contentsMargins().top()
            + funHLayout->contentsMargins().bottom();
    funWidget->setFixedSize(w, h);
}

void MessageWidget::setText(const QString &text)
{
    qDebug() << "MessageWidget setText start" << this;
    this->text = text;
    if (isUser) {
        if (textShow)
            textShow->setText(this->text);
        // setSize();
        return;
    }

    emit setTexting(true);

    parseThinkAndResult(this->text, thinkText, resultText, thinkTextIsRecvEnd);
    qDebug() << "MessageWidget setText ing0" << this;
    if (!thinkText.isEmpty() && !QString("</think>").contains(thinkText)) {
        if (!thinkButtonHaveCreated) {
            thinkButton = new ThinkingButton(appContext);
            thinkButton->setIsShowThinkContent(thinkIsExpand);
            thinkButton->connectButtonClick(this, &MessageWidget::thinkButtonClicked);
            thinkButtonHaveCreated = true;
            textLayout->addWidget(thinkButton);
            textLayout->addWidget(thinkBackWidget);
            thinkBackWidget->setVisible(thinkIsExpand);
        }

        qDebug() << "MessageWidget setText ing1" << this;
        auto thinkCodeBlocks = extractCodeBlocks(thinkText);
        QStringList thinkSplitTextList;
        QString thinkTempText = thinkText;
        qDebug() << "setText thinkText:" << thinkText << this;
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
                QPointer<MessageWidget> thinkCodeSelf(this);
                auto *codeShow = new CodeShow(
                        appContext, thinkCodeBlocks[i].code, thinkCodeBlocks[i].language,
                        [thinkCodeSelf]() {
                            if (thinkCodeSelf)
                                thinkCodeSelf->onSizeFinished();
                        },
                        textMaxWidth - imageLabel->width() - 80, this);
                codeShow->hide();
                codeShow->connectCodeCopyButtonClick(copyFun);
                // codeShow->setVisible(thinkIsExpand);
                thinkCodeShowList.append(codeShow);
            } else {
                thinkCodeShowList[i]->setText(thinkCodeBlocks[i].code, thinkCodeBlocks[i].language);
                qDebug() << "thinkCodeShowList setText end";
            }
            qDebug() << "thinkCodeBlocks[" << i << "]:" << thinkCodeBlocks[i].language
                     << thinkCodeBlocks[i].code.left(5) << "and" << thinkCodeBlocks[i].code.right(5)
                     << "and" << thinkCodeBlocks[i].endMarker << "end";
        }
        thinkSplitTextList.append(thinkTempText);
        qDebug() << "thinkSplitTextList:" << thinkSplitTextList;
        int i = 0;
        int thinkTextShowListLastLen = thinkTextShowList.size() - 1;
        qDebug() << "thinkTextShowListLastLen:" << thinkTextShowListLastLen;
        for (const auto &splitText : thinkSplitTextList) {
            if (!splitText.trimmed().isEmpty()) {
                if (thinkTextShowListLastLen < i) {
                    QPointer<MessageWidget> thinkTextSelf(this);
                    ThinkWidget *thinkWidget = new ThinkWidget(
                            appContext, splitText,
                            [thinkTextSelf]() {
                                if (thinkTextSelf)
                                    thinkTextSelf->onSizeFinished();
                            },
                            textMaxWidth - imageLabel->width() - 80, this);
                    // thinkWidget->setVisible(thinkIsExpand);
                    thinkTextShowList.append(thinkWidget);
                    // connect(thinkWidget, &ThinkWidget::setSizeFinished, this,
                    //         &MessageWidget::onSizeFinished);
                    // if (thinkWidget->getIsEmitSizeFinish()) {
                    //     thinkWidget->setIsEmitSizeFinish(false);
                    //     emit thinkWidget->setSizeFinished();
                    // }
                } else {
                    if (splitText.trimmed() != thinkTextShowList[i]->getText()) {
                        // QEventLoop externalLoop, internalLoop;
                        // QTimer processTimer;
                        // processTimer.setInterval(2);
                        // processQueue.enqueue(&processTimer);
                        // QMetaObject::Connection processConnection =
                        //         QObject::connect(&processTimer, &QTimer::timeout, [&]() {
                        //             if (!processQueue.isEmpty()
                        //                 && processQueue.head() == &processTimer) {
                        //                 bool ok = QObject::disconnect(processConnection);
                        //                 qDebug() << "disconnect2 processConnection" << ok;
                        //                 processTimer.stop();
                        thinkTextShowList[i]->setText(splitText);
                        // qDebug() << "internalLoop:" << &internalLoop
                        //          << "externalLoop:" << &externalLoop
                        //          << "checkTimer:" << &checkTimer
                        //          << "processTimer:" << &processTimer;
                        // 防重入：同一 QEventLoop 实例嵌套 exec() 会返回 -1 且产生悬空 lambda，重入时跳过等待
                        if (!loop.isRunning()) {
                            QMetaObject::Connection eventLoopQuitConnect =
                                    QObject::connect(&checkTimer, &QTimer::timeout, [&]() {
                                        if (thinkTextShowList[i]->getIsSizeFinish()) {
                                            qDebug() << "getIsSizeFinish before:"
                                                     << thinkTextShowList[i]->getIsSizeFinish()
                                                     << thinkTextShowList[i];
                                            thinkTextShowList[i]->setIsSizeFinish(false);
                                            qDebug() << "getIsSizeFinish after:"
                                                     << thinkTextShowList[i]->getIsSizeFinish()
                                                     << thinkTextShowList[i];
                                            bool ok = QObject::disconnect(eventLoopQuitConnect);
                                            qDebug() << "disconnect2" << ok;
                                            checkTimer.stop();
                                            // qDebug()
                                            //         << "internalLoop:" << &internalLoop
                                            //         << "externalLoop:" << &externalLoop
                                            //         << "checkTimer:" << &checkTimer
                                            //         << "processTimer:" << &processTimer;
                                            loop.quit();
                                            qDebug() << "loop quit2";
                                        }
                                    });
                            checkTimer.start();
                            qDebug() << "checkTimer start2";
                            loop.exec();
                            qDebug() << "loop exec2";
                        }
                        // externalLoop.quit();
                        // qDebug() << "externalLoop quit";
                        //             }
                        //         });
                        // qDebug() << "internalLoop:" << &internalLoop
                        //          << "externalLoop:" << &externalLoop << "checkTimer:" << &checkTimer
                        //          << "processTimer:" << &processTimer;
                        // processTimer.start();
                        // qDebug() << "processTimer start2";
                        // externalLoop.exec();
                        // qDebug() << "externalLoop exec2";
                        // processQueue.dequeue();
                    }
                }
                i += 1;
            }
        }
        int j = 0;
        for (int i = 0; i < thinkCodeShowList.size(); ++i) {
            if (!thinkSplitTextList[i].trimmed().isEmpty()) {
                if (thinkTextShowListLastLen < i - j) {
                    // QEventLoop externalLoop, internalLoop;
                    // QTimer processTimer;
                    // processTimer.setInterval(2);
                    // processQueue.enqueue(&processTimer);
                    // QMetaObject::Connection processConnection =
                    //         QObject::connect(&processTimer, &QTimer::timeout, [&]() {
                    //             if (!processQueue.isEmpty()
                    //                 && processQueue.head() == &processTimer) {
                    //                 bool ok = QObject::disconnect(processConnection);
                    //                 qDebug() << "disconnect processConnection" << ok;
                    //                 processTimer.stop();
                    thinkBackVLayout->addWidget(thinkTextShowList[i - j]);
                    // qDebug() << "internalLoop:" << &internalLoop
                    //          << "externalLoop:" << &externalLoop
                    //          << "checkTimer:" << &checkTimer
                    //          << "processTimer:" << &processTimer;
                    // 防重入：同一 QEventLoop 实例嵌套 exec() 会返回 -1 且产生悬空 lambda，重入时跳过等待
                    if (!loop.isRunning()) {
                        QMetaObject::Connection eventLoopQuitConnect =
                                QObject::connect(&checkTimer, &QTimer::timeout, [&]() {
                                    if (thinkTextShowList[i - j]->getIsSizeFinish()) {
                                        qDebug() << "getIsSizeFinish before:"
                                                 << thinkTextShowList[i - j]->getIsSizeFinish()
                                                 << thinkTextShowList[i - j];
                                        thinkTextShowList[i - j]->setIsSizeFinish(false);
                                        qDebug() << "getIsSizeFinish after:"
                                                 << thinkTextShowList[i - j]->getIsSizeFinish()
                                                 << thinkTextShowList[i - j];
                                        bool ok = QObject::disconnect(eventLoopQuitConnect);
                                        qDebug() << "disconnect" << ok;
                                        checkTimer.stop();
                                        // qDebug() << "internalLoop:" << &internalLoop
                                        //          << "externalLoop:" << &externalLoop
                                        //          << "checkTimer:" << &checkTimer
                                        //          << "processTimer:" << &processTimer;
                                        loop.quit();
                                        qDebug() << "loop quit";
                                    }
                                });
                        checkTimer.start();
                        qDebug() << "checkTimer start";
                        loop.exec();
                        qDebug() << "loop exec";
                    }
                    // externalLoop.quit();
                    // qDebug() << "externalLoop quit";
                    //             }
                    //         });
                    // qDebug() << "internalLoop:" << &internalLoop << "externalLoop:" << &externalLoop
                    //          << "checkTimer:" << &checkTimer << "processTimer:" << &processTimer;
                    // processTimer.start();
                    // qDebug() << "processTimer start";
                    // externalLoop.exec();
                    // qDebug() << "externalLoop exec";
                    // processQueue.dequeue();
                }
            } else {
                j += 1;
            }
            if (thinkCodeShowListLastLen < i) {
                thinkBackVLayout->addWidget(thinkCodeShowList[i]);
                thinkCodeShowList[i]->show();
                onSizeFinished();
                qDebug() << "thinkCodeShowList addWidget end";
            }
        }
        qDebug() << "thinkTextShowListLastLen thinkTextShowList.size j:" << thinkTextShowListLastLen
                 << thinkTextShowList.size() << j;
        qDebug() << "准备添加最后ThinkWidget";
        if (thinkTextShowListLastLen < thinkTextShowList.size() - 1
            && !thinkSplitTextList.last().isEmpty()) {
            // QEventLoop externalLoop, internalLoop;
            // QTimer processTimer;
            // processTimer.setInterval(2);
            // processQueue.enqueue(&processTimer);
            // QMetaObject::Connection processConnection =
            //         QObject::connect(&processTimer, &QTimer::timeout, [&]() {
            //             if (!processQueue.isEmpty() && processQueue.head() == &processTimer) {
            //                 bool ok = QObject::disconnect(processConnection);
            //                 qDebug() << "disconnect1 processConnection" << ok;
            //                 processTimer.stop();
            thinkBackVLayout->addWidget(thinkTextShowList.last());
            // qDebug() << "internalLoop:" << &internalLoop
            //          << "externalLoop:" << &externalLoop
            //          << "checkTimer:" << &checkTimer
            //          << "processTimer:" << &processTimer;
            // 防重入：同一 QEventLoop 实例嵌套 exec() 会返回 -1 且产生悬空 lambda，重入时跳过等待
            if (!loop.isRunning()) {
                QMetaObject::Connection eventLoopQuitConnect =
                        QObject::connect(&checkTimer, &QTimer::timeout, [&]() {
                            if (thinkTextShowList.last()->getIsSizeFinish()) {
                                qDebug() << "getIsSizeFinish before:"
                                         << thinkTextShowList.last()->getIsSizeFinish()
                                         << thinkTextShowList.last();
                                thinkTextShowList.last()->setIsSizeFinish(false);
                                qDebug() << "getIsSizeFinish after:"
                                         << thinkTextShowList.last()->getIsSizeFinish()
                                         << thinkTextShowList.last();
                                bool ok = QObject::disconnect(eventLoopQuitConnect);
                                qDebug() << "disconnect1" << ok;
                                checkTimer.stop();
                                // qDebug() << "internalLoop:" << &internalLoop
                                //          << "externalLoop:" << &externalLoop
                                //          << "checkTimer:" << &checkTimer
                                //          << "processTimer:" << &processTimer;
                                loop.quit();
                                qDebug() << "loop quit1";
                            }
                        });
                checkTimer.start();
                qDebug() << "checkTimer start1";
                loop.exec();
                qDebug() << "loop exec1";
            }
            // externalLoop.quit();
            // qDebug() << "externalLoop quit1";
            //             }
            //         });
            // qDebug() << "internalLoop:" << &internalLoop << "externalLoop:" << &externalLoop
            //          << "checkTimer:" << &checkTimer << "processTimer:" << &processTimer;
            // processTimer.start();
            // qDebug() << "processTimer start1";
            // externalLoop.exec();
            // qDebug() << "externalLoop exec1";
            // processQueue.dequeue();
        }
        qDebug() << "添加完最后ThinkWidget";
        if (thinkTextIsRecvEnd && isRecvFirst) {
            thinkButton->setThinkEnd();
            isRecvFirst = false;
        }
    }

    if (!resultText.isEmpty()) {
        qDebug() << "MessageWidget setText ing2" << this;
        auto resultCodeBlocks = extractCodeBlocks(resultText);
        QStringList resultSplitTextList;
        QString resultTempText = resultText;
        qDebug() << "setText resultText:" << resultText << this;
        int resultCodeShowListLastLen = resultCodeShowList.size() - 1;
        for (int i = 0; i < resultCodeBlocks.size(); i++) {
            int idx = resultTempText.indexOf(QString("```%1\n").arg(resultCodeBlocks[i].language));
            resultSplitTextList.append(resultTempText.left(idx));
            resultTempText = resultTempText.mid(
                    idx + QString("```%1\n").arg(resultCodeBlocks[i].language).size()
                    + resultCodeBlocks[i].code.size() + 3);
            if (resultCodeShowListLastLen < i) {
                qDebug() << "MessageWidget setText ing3" << this;
                QPointer<MessageWidget> resultCodeSelf(this);
                auto *codeShow = new CodeShow(
                        appContext, resultCodeBlocks[i].code, resultCodeBlocks[i].language,
                        [resultCodeSelf]() {
                            if (resultCodeSelf)
                                resultCodeSelf->onSizeFinished();
                        },
                        textMaxWidth - imageLabel->width() - 35, this);
                codeShow->hide();
                qDebug() << "MessageWidget setText ing4" << this;
                codeShow->connectCodeCopyButtonClick(copyFun);
                resultCodeShowList.append(codeShow);
                qDebug() << "MessageWidget setText ing5" << this;
            } else {
                resultCodeShowList[i]->setText(resultCodeBlocks[i].code,
                                               resultCodeBlocks[i].language);
            }
        }
        resultSplitTextList.append(resultTempText);
        int i = 0;
        int resultTextShowListLastLen = resultTextShowList.size() - 1;
        for (const auto &splitText : resultSplitTextList) {
            if (!splitText.trimmed().isEmpty()) {
                if (resultTextShowListLastLen < i) {
                    qDebug() << "messageWidget splitText:" << splitText;
                    QPointer<MessageWidget> resultTextSelf(this);
                    resultTextShowList.append(new TextShow(
                            appContext, splitText,
                            [resultTextSelf]() {
                                if (resultTextSelf)
                                    resultTextSelf->onSizeFinished();
                            },
                            nullptr, textMaxWidth - imageLabel->width() - 35, this));
                    // connect(resultTextShowList.last(), &ThinkWidget::setSizeFinished, this,
                    //         &MessageWidget::onSizeFinished);
                    // if (resultTextShowList.last()->getIsEmitSizeFinish()) {
                    //     resultTextShowList.last()->setIsEmitSizeFinish(false);
                    //     emit resultTextShowList.last()->setSizeFinished();
                    // }
                } else {
                    if (splitText.trimmed() != resultTextShowList[i]->getText()) {
                        // QEventLoop externalLoop, internalLoop;
                        // QTimer processTimer;
                        // processTimer.setInterval(2);
                        // processQueue.enqueue(&processTimer);
                        // QMetaObject::Connection processConnection =
                        //         QObject::connect(&processTimer, &QTimer::timeout, [&]() {
                        //             if (!processQueue.isEmpty()
                        //                 && processQueue.head() == &processTimer) {
                        //                 bool ok = QObject::disconnect(processConnection);
                        //                 qDebug() << "disconnect2 processConnection" << ok;
                        //                 processTimer.stop();
                        resultTextShowList[i]->setText(splitText);
                        // qDebug() << "internalLoop:" << &internalLoop
                        //          << "externalLoop:" << &externalLoop
                        //          << "checkTimer:" << &checkTimer
                        //          << "processTimer:" << &processTimer;
                        // 防重入：同一 QEventLoop 实例嵌套 exec() 会返回 -1 且产生悬空 lambda，重入时跳过等待
                        if (!loop.isRunning()) {
                            QMetaObject::Connection eventLoopQuitConnect =
                                    QObject::connect(&checkTimer, &QTimer::timeout, [&]() {
                                        if (resultTextShowList[i]->getIsSizeFinish()) {
                                            resultTextShowList[i]->setIsSizeFinish(false);
                                            bool ok = QObject::disconnect(eventLoopQuitConnect);
                                            qDebug() << "disconnect2" << ok;
                                            checkTimer.stop();
                                            // qDebug()
                                            //         << "internalLoop:" << &internalLoop
                                            //         << "externalLoop:" << &externalLoop
                                            //         << "checkTimer:" << &checkTimer
                                            //         << "processTimer:" << &processTimer;
                                            loop.quit();
                                            qDebug() << "loop quit2";
                                        }
                                    });
                            checkTimer.start();
                            qDebug() << "checkTimer start2";
                            loop.exec();
                            qDebug() << "loop exec2";
                        }
                        // externalLoop.quit();
                        // qDebug() << "externalLoop quit2";
                        //             }
                        //         });
                        // qDebug() << "internalLoop:" << &internalLoop
                        //          << "externalLoop:" << &externalLoop << "checkTimer:" << &checkTimer
                        //          << "processTimer:" << &processTimer;
                        // processTimer.start();
                        // qDebug() << "processTimer start2";
                        // externalLoop.exec();
                        // qDebug() << "externalLoop exec2";
                        // processQueue.dequeue();
                    }
                }
                i += 1;
            }
        }
        int j = 0;
        for (int i = 0; i < resultCodeShowList.size(); ++i) {
            if (!resultSplitTextList[i].trimmed().isEmpty()) {
                if (resultTextShowListLastLen < i - j) {
                    // QEventLoop externalLoop, internalLoop;
                    // QTimer processTimer;
                    // processTimer.setInterval(2);
                    // processQueue.enqueue(&processTimer);
                    // QMetaObject::Connection processConnection =
                    //         QObject::connect(&processTimer, &QTimer::timeout, [&]() {
                    //             if (!processQueue.isEmpty()
                    //                 && processQueue.head() == &processTimer) {
                    //                 bool ok = QObject::disconnect(processConnection);
                    //                 qDebug() << "disconnect processConnection" << ok;
                    //                 processTimer.stop();
                    textLayout->addWidget(resultTextShowList[i - j]);
                    // qDebug() << "internalLoop:" << &internalLoop
                    //          << "externalLoop:" << &externalLoop
                    //          << "checkTimer:" << &checkTimer
                    //          << "processTimer:" << &processTimer;
                    // 防重入：同一 QEventLoop 实例嵌套 exec() 会返回 -1 且产生悬空 lambda，重入时跳过等待
                    if (!loop.isRunning()) {
                        QMetaObject::Connection eventLoopQuitConnect =
                                QObject::connect(&checkTimer, &QTimer::timeout, [&]() {
                                    if (resultTextShowList[i - j]->getIsSizeFinish()) {
                                        resultTextShowList[i - j]->setIsSizeFinish(false);
                                        bool ok = QObject::disconnect(eventLoopQuitConnect);
                                        qDebug() << "disconnect" << ok;
                                        checkTimer.stop();
                                        // qDebug() << "internalLoop:" << &internalLoop
                                        //          << "externalLoop:" << &externalLoop
                                        //          << "checkTimer:" << &checkTimer
                                        //          << "processTimer:" << &processTimer;
                                        loop.quit();
                                        qDebug() << "loop quit";
                                    }
                                });
                        checkTimer.start();
                        qDebug() << "checkTimer start";
                        loop.exec();
                        qDebug() << "loop exec";
                    }
                    // externalLoop.quit();
                    // qDebug() << "externalLoop quit";
                    //             }
                    //         });
                    // qDebug() << "internalLoop:" << &internalLoop << "externalLoop:" << &externalLoop
                    //          << "checkTimer:" << &checkTimer << "processTimer:" << &processTimer;
                    // processTimer.start();
                    // qDebug() << "processTimer start";
                    // externalLoop.exec();
                    // qDebug() << "externalLoop exec";
                    // processQueue.dequeue();
                }
            } else {
                j += 1;
            }
            if (resultCodeShowListLastLen < i) {
                textLayout->addWidget(resultCodeShowList[i]);
                resultCodeShowList[i]->show();
                onSizeFinished();
            }
        }
        if (resultTextShowListLastLen < resultTextShowList.size() - 1
            && !resultSplitTextList.last().isEmpty()) {
            // QEventLoop externalLoop, internalLoop;
            // QTimer processTimer;
            // processTimer.setInterval(2);
            // processQueue.enqueue(&processTimer);
            // QMetaObject::Connection processConnection =
            //         QObject::connect(&processTimer, &QTimer::timeout, [&]() {
            //             if (!processQueue.isEmpty() && processQueue.head() == &processTimer) {
            //                 bool ok = QObject::disconnect(processConnection);
            //                 qDebug() << "disconnect1 processConnection" << ok;
            //                 processTimer.stop();
            textLayout->addWidget(resultTextShowList.last());
            // qDebug() << "internalLoop:" << &internalLoop
            //          << "externalLoop:" << &externalLoop
            //          << "checkTimer:" << &checkTimer
            //          << "processTimer:" << &processTimer;
            // 防重入：同一 QEventLoop 实例嵌套 exec() 会返回 -1 且产生悬空 lambda，重入时跳过等待
            if (!loop.isRunning()) {
                QMetaObject::Connection eventLoopQuitConnect =
                        QObject::connect(&checkTimer, &QTimer::timeout, [&]() {
                            if (resultTextShowList.last()->getIsSizeFinish()) {
                                resultTextShowList.last()->setIsSizeFinish(false);
                                bool ok = QObject::disconnect(eventLoopQuitConnect);
                                qDebug() << "disconnect1" << ok;
                                checkTimer.stop();
                                // qDebug() << "internalLoop:" << &internalLoop
                                //          << "externalLoop:" << &externalLoop
                                //          << "checkTimer:" << &checkTimer
                                //          << "processTimer:" << &processTimer;
                                loop.quit();
                                qDebug() << "loop quit1";
                            }
                        });
                checkTimer.start();
                qDebug() << "checkTimer start1";
                loop.exec();
                qDebug() << "loop exec1";
            }
            // externalLoop.quit();
            // qDebug() << "externalLoop quit1";
            //             }
            //         });
            // qDebug() << "internalLoop:" << &internalLoop << "externalLoop:" << &externalLoop
            //          << "checkTimer:" << &checkTimer << "processTimer:" << &processTimer;
            // processTimer.start();
            // qDebug() << "processTimer start1";
            // externalLoop.exec();
            // qDebug() << "externalLoop exec1";
            // processQueue.dequeue();
        }
    }

    // syncThinkTimeLength();
    // setSize();
    // emit resizeFinished(this);
    emit setTexting(false);
    qDebug() << "MessageWidget setText end";
}

void MessageWidget::removeLoadingWidget()
{
    if (isUser || loadingWidgetIsRemove)
        return;
    textBoxLayout->removeWidget(loadingWidget);
    loadingWidget->deleteLater();
    loadingWidgetIsRemove = true;
    textBoxLayout->addWidget(funWidget);
    textBoxLayout->setSpacing(0);
    textBoxWidget->setFixedSize(qMax(textWidget->width(), funWidget->width()),
                                textWidget->height() + funWidget->height());
    setFixedSize(imageLabel->width() + textBoxWidget->width() + 5,
                 qMax(imageLabel->height(), textBoxWidget->height()));
}

void MessageWidget::onSizeFinished()
{
    static int i;
    i += 1;
    qDebug() << "onSizeFinished" << i << this;
    // ThinkWidget *thinkWidget = qobject_cast<ThinkWidget *>(sender());
    // if (thinkWidget->getIsEmitSizeFinish())
    //     thinkWidget->setIsEmitSizeFinish(false);
    // if (isUser)
    //     emit resizeFinished();

    if (!isUser) {
        // bool isContinue = false;
        // if (CodeShow *codeShow = qobject_cast<CodeShow *>(sender())) {
        //     for (auto *oneCodeShow :
        //          thinkBackWidget->findChildren<CodeShow *>(Qt::FindDirectChildrenOnly)) {
        //         if (codeShow == oneCodeShow) {
        //             isContinue = true;
        //             break;
        //         }
        //     }
        //     if (!isContinue) {
        //         for (auto *oneCodeShow :
        //              textWidget->findChildren<CodeShow *>(Qt::FindDirectChildrenOnly)) {
        //             if (codeShow == oneCodeShow) {
        //                 isContinue = true;
        //                 break;
        //             }
        //         }
        //     }
        // } else if (ThinkWidget *thinkWidget = qobject_cast<ThinkWidget *>(sender())) {
        //     for (auto *oneThinkWidget :
        //          thinkBackWidget->findChildren<ThinkWidget *>(Qt::FindDirectChildrenOnly)) {
        //         if (thinkWidget == oneThinkWidget) {
        //             isContinue = true;
        //             break;
        //         }
        //     }
        //     if (!isContinue) {
        //         for (auto *oneTextShow :
        //              textWidget->findChildren<TextShow *>(Qt::FindDirectChildrenOnly)) {
        //             if (thinkWidget == qobject_cast<ThinkWidget *>(oneTextShow)) {
        //                 isContinue = true;
        //                 break;
        //             }
        //         }
        //     }
        // }
        // if (!isContinue)
        //     return;

        syncThinkTimeLength();
    }
    qDebug() << "onSizeFinished setSize before";
    setSize();
    qDebug() << "onSizeFinished resizeFinished before";
    emit resizeFinished(this);
    qDebug() << "onSizeFinished resizeFinished after";
}

ListWidget *MessageWidget::getListWidget()
{
    return listWidget;
}

bool MessageWidget::hasSelectedText()
{
    qDebug() << "MessageWidget::hasSelectedText 1";
    if (isUser) {
        qDebug() << "textShow->hasSelectedText():" << textShow->hasSelectedText();
        return textShow ? textShow->hasSelectedText() : false;
    }
    qDebug() << "MessageWidget::hasSelectedText 2";
    for (auto *thinkWidget : thinkTextShowList)
        if (thinkWidget->hasSelectedText())
            return true;
    qDebug() << "MessageWidget::hasSelectedText 3";
    for (auto *codeShow : thinkCodeShowList)
        if (codeShow->hasSelectedText())
            return true;
    qDebug() << "MessageWidget::hasSelectedText 4";
    for (auto *textShow : resultTextShowList)
        if (textShow->hasSelectedText())
            return true;
    qDebug() << "MessageWidget::hasSelectedText 5";
    for (auto *codeShow : resultCodeShowList)
        if (codeShow->hasSelectedText())
            return true;
    qDebug() << "MessageWidget::hasSelectedText 6";
    return false;
}

QString MessageWidget::getSelectedText()
{
    qDebug() << "MessageWidget::getSelectedText 1";
    if (isUser) {
        return textShow ? textShow->getSelectedText() : "";
    }
    qDebug() << "MessageWidget::getSelectedText 2";
    for (auto *thinkWidget : thinkTextShowList)
        if (thinkWidget->hasSelectedText())
            return thinkWidget->getSelectedText();
    qDebug() << "MessageWidget::getSelectedText 3";
    for (auto *codeShow : thinkCodeShowList)
        if (codeShow->hasSelectedText())
            return codeShow->getSelectedText();
    qDebug() << "MessageWidget::getSelectedText 4";
    for (auto *textShow : resultTextShowList)
        if (textShow->hasSelectedText())
            return textShow->getSelectedText();
    qDebug() << "MessageWidget::getSelectedText 5";
    for (auto *codeShow : resultCodeShowList)
        if (codeShow->hasSelectedText())
            return codeShow->getSelectedText();
    qDebug() << "MessageWidget::getSelectedText 6";
    return "";
}
