#include "textedit.h"

#include <QDebug>

TextEdit::TextEdit(QWidget *parent) : QTextEdit(parent), isSending(false)
{
    resize(1130, 150);
    setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    int font_id = QFontDatabase::addApplicationFont(fontFilePath);
    if (font_id != -1) {
        QStringList font_families = QFontDatabase::applicationFontFamilies(font_id);
        if (!font_families.isEmpty()) {
            QString font_family = font_families.at(0);
            QFont font(font_family);
            font.setPixelSize(windowFontPixelSize);
            setFont(font);
            qDebug() << "textEdit setFont end";
        }
    }
    setPlaceholderText("按Shift+Enter换行、按Enter提交");
    sendButton = new SendButton("发送", 10, 40);
    sendButton->setFixedSize(30, 30);
    sendButton->setIconSize(QSize(30, 30));
    gLayout = new QGridLayout();
    setLayout(gLayout);
    gLayout->setRowStretch(0, 1);
    gLayout->setColumnStretch(0, 1);
    gLayout->addWidget(sendButton, 1, 1);
    gLayout->setContentsMargins(10, 10, 25, 10);
    gLayout->setSpacing(0);
    connect(this, &QTextEdit::textChanged, this, &TextEdit::sendButtonShow);
    sendImagesPath = imagesDir + "/send.png";
    sendHoverImagesPath = imagesDir + "/send_hover.png";
    sendDisableImagesPath = imagesDir + "/send_disable.png";
    sendButton->setStyleSheet(QString(R"(
        QPushButton{
            border: none;
            image: url("%1");
        }
    )")
                                      .arg(sendImagesPath));
    setStyleSheet(QString(R"(
        QTextEdit{
            border: none;
            background: transparent;
            font-size: %1pt;
        }
        QScrollBar{
            width: 25px;
        }
    )")
                          .arg(windowFontPointSize));
    setMouseTracking(true);
    widgetSizeDict["sendButton"] = sendButton->size();
    widgetSizeDict["sendButton iconSize"] = sendButton->iconSize();
}

TextEdit::~TextEdit() { }

void TextEdit::contextMenuEvent(QContextMenuEvent *e)
{
    CustomMenu *menu = new CustomMenu(this);
    menu->setStyleSheet(R"(
        QMenu {
            background-color: white;
            border: none;
            border-radius: 15px;
            padding: 5px;
        }
        QMenu::item:selected {
            color: black;
            background-color: #e0e0e0;
        }
    )");
    QAction *action1 = menu->addAction("剪切");
    action1->setShortcut(QKeySequence("Ctrl+x"));
    QString action1ImagesPath = imagesDir + "/cut.png";
    action1->setIcon(QIcon(action1ImagesPath));
    connect(action1, &QAction::triggered, this, &QTextEdit::cut);

    QAction *action2 = menu->addAction("复制");
    action2->setShortcut(QKeySequence("Ctrl+c"));
    QString action2ImagesPath = imagesDir + "/menu_copy.png";
    action2->setIcon(QIcon(action2ImagesPath));
    connect(action2, &QAction::triggered, this, &QTextEdit::copy);

    QAction *action3 = menu->addAction("粘贴");
    action3->setShortcut(QKeySequence("Ctrl+v"));
    QString action3ImagesPath = imagesDir + "/paste.png";
    action3->setIcon(QIcon(action3ImagesPath));
    connect(action3, &QAction::triggered, this, &QTextEdit::paste);

    menu->exec(e->globalPos());
    delete menu;
}

void TextEdit::mouseMoveEvent(QMouseEvent *e)
{
    QTextEdit::mouseMoveEvent(e);
    e->ignore();
}

void TextEdit::mousePressEvent(QMouseEvent *e)
{
    QTextEdit::mousePressEvent(e);
    e->ignore();
}

void TextEdit::mouseReleaseEvent(QMouseEvent *e)
{
    QTextEdit::mouseReleaseEvent(e);
    e->ignore();
}

void TextEdit::keyPressEvent(QKeyEvent *e)
{
    if (e->key() == Qt::Key_Return) {
        if (e->modifiers() == Qt::ShiftModifier) {
            QTextEdit::keyPressEvent(e);
        } else {
            emitSendButtonClicked();
            e->accept();
        }
    } else if (e->key() == Qt::Key_Enter) {
        emitSendButtonClicked();
        e->accept();
    } else {
        QTextEdit::keyPressEvent(e);
    }
}

void TextEdit::updateSendButtonSize(qreal curDpi, qreal lastDpi)
{
    if (lastDpi == 0)
        return;
    qreal ratio = qreal(curDpi / lastDpi);

    auto scale = [=](const QSize &s) -> QSize {
        return QSize(qRound(s.width() * ratio), qRound(s.height() * ratio));
    };

    sendButton->setFixedSize(scale(widgetSizeDict["sendButton"].value<QSize>()));
    sendButton->setIconSize(scale(widgetSizeDict["sendButton iconSize"].value<QSize>()));

    widgetSizeDict["sendButton"] = sendButton->size();
    widgetSizeDict["sendButton iconSize"] = sendButton->iconSize();
}

void TextEdit::emitSendButtonClicked()
{
    emit sendButton->clicked();
}

void TextEdit::sendButtonShow()
{
    if (isSending) {
        sendButton->setStyleSheet(QString(R"(
            QPushButton{
                border: none;
                image: url("%1");
            }
        )")
                                          .arg(sendDisableImagesPath));
    } else if (toPlainText().isEmpty()) {
        sendButton->setStyleSheet(QString(R"(
            QPushButton{
                border: none;
                image: url("%1");
            }
        )")
                                          .arg(sendImagesPath));
    } else {
        sendButton->setStyleSheet(QString(R"(
            QPushButton{
                border: none;
                image: url("%1");
            }
        )")
                                          .arg(sendHoverImagesPath));
    }
}

void TextEdit::setSending(bool isSending)
{
    this->isSending = isSending;
    emit textChanged();
}
