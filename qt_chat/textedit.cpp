#include "textedit.h"

TextEdit::TextEdit(QWidget *parent)
    : QTextEdit(parent), isSending(false)
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
    send_images_path = imagesDir + "/send.png";
    send_hover_images_path = imagesDir + "/send_hover.png";
    send_disable_images_path = imagesDir + "/send_disable.png";
    sendButton->setStyleSheet(QString(R"(
        QPushButton{
            border: none;
            image: url("%1");
        }
    )").arg(send_images_path));
    setStyleSheet(QString(R"(
        QTextEdit{
            border: none;
            background: transparent;
            font-size: %1pt;
        }
        QScrollBar{
            width: 25px;
        }
    )").arg(windowFontPointSize));
    setMouseTracking(true);
    widgetSizeDict["sendButton"] = sendButton->size();
    widgetSizeDict["sendButton iconSize"] = sendButton->iconSize();
}

TextEdit::~TextEdit()
{
}

void TextEdit::contextMenuEvent(QContextMenuEvent *event)
{
    QMenu *menu = new CustomMenu(this); // 假设有 CustomMenu 类
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
    QString action1_images_path = imagesDir + "/cut.png";
    action1->setIcon(QIcon(action1_images_path));
    connect(action1, &QAction::triggered, this, &QTextEdit::cut);

    QAction *action2 = menu->addAction("复制");
    action2->setShortcut(QKeySequence("Ctrl+c"));
    QString action2_images_path = imagesDir + "/menu_copy.png";
    action2->setIcon(QIcon(action2_images_path));
    connect(action2, &QAction::triggered, this, &QTextEdit::copy);

    QAction *action3 = menu->addAction("粘贴");
    action3->setShortcut(QKeySequence("Ctrl+v"));
    QString action3_images_path = imagesDir + "/paste.png";
    action3->setIcon(QIcon(action3_images_path));
    connect(action3, &QAction::triggered, this, &QTextEdit::paste);

    menu->exec(event->globalPos());
    delete menu;
}

void TextEdit::mouseMoveEvent(QMouseEvent *event)
{
    QTextEdit::mouseMoveEvent(event);
    event->ignore();
}

void TextEdit::mousePressEvent(QMouseEvent *event)
{
    QTextEdit::mousePressEvent(event);
    event->ignore();
}

void TextEdit::mouseReleaseEvent(QMouseEvent *event)
{
    QTextEdit::mouseReleaseEvent(event);
    event->ignore();
}

void TextEdit::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Return) {
        if (event->modifiers() == Qt::ShiftModifier) {
            QTextEdit::keyPressEvent(event);
        } else {
            emitSendButtonClicked();
            event->accept();
        }
    } else if (event->key() == Qt::Key_Enter) {
        emitSendButtonClicked();
        event->accept();
    } else {
        QTextEdit::keyPressEvent(event);
    }
}

void TextEdit::updateSendButtonSize(qreal curDpi, qreal lastDpi)
{
    if (lastDpi == 0) return;
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
        )").arg(send_disable_images_path));
    } else if (toPlainText().isEmpty()) {
        sendButton->setStyleSheet(QString(R"(
            QPushButton{
                border: none;
                image: url("%1");
            }
        )").arg(send_images_path));
    } else {
        sendButton->setStyleSheet(QString(R"(
            QPushButton{
                border: none;
                image: url("%1");
            }
        )").arg(send_hover_images_path));
    }
}

void TextEdit::setSending(bool isSending)
{
    isSending = true;
    emit textChanged();
}
