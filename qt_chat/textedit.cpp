#include "textedit.h"
#include "appcontext.h"
#include "dpiutil.h"

#include <QDebug>
#include <QtGui/QScreen>

namespace {
// chatInput 内部各部件以 144 DPI（150% 缩放）屏上的物理尺寸为基准（DpiUtil::
// kBaseDpi），其它 DPI 按 dpi / 144 换算后取整，让两档屏看到的物理大小一致
// （96 DPI 正好是 2/3：发送按钮 30→20、内部布局边距 10/25→7/17、滚动条 25→17）
constexpr int kBaseSendButtonSize = 30;
constexpr int kBaseGLayoutLeft = 10;
constexpr int kBaseGLayoutTop = 10;
constexpr int kBaseGLayoutRight = 25;
constexpr int kBaseGLayoutBottom = 10;
constexpr int kBaseScrollBarWidth = 25;
} // namespace

TextEdit::TextEdit(AppContext *appContext, QWidget *parent)
    : QTextEdit(parent), appContext(appContext), isSending(false), appliedDpi(-1),
      appliedFontPixelSize(-1)
{
    resize(1130, kBaseTextHeight);
    setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    // 字体族由 AppContext 统一注册缓存，直接取用（不再逐个 addApplicationFont）
    const QString fontFamily = appContext->fontFamily();
    if (!fontFamily.isEmpty()) {
        font = QFont(fontFamily);
        font.setPixelSize(appContext->windowFontPixelSize());
        setFont(font);
        qDebug() << "textEdit setFont end";
    }
    setPlaceholderText("按Shift+Enter换行、按Enter提交");
    sendButton = new SendButton(this->appContext, "发送");
    sendButton->setFixedSize(kBaseSendButtonSize, kBaseSendButtonSize);
    sendButton->setIconSize(QSize(kBaseSendButtonSize, kBaseSendButtonSize));
    gLayout = new QGridLayout();
    setLayout(gLayout);
    gLayout->setRowStretch(0, 1);
    gLayout->setColumnStretch(0, 1);
    gLayout->addWidget(sendButton, 1, 1);
    gLayout->setContentsMargins(kBaseGLayoutLeft, kBaseGLayoutTop, kBaseGLayoutRight,
                                kBaseGLayoutBottom);
    gLayout->setSpacing(0);
    connect(this, &QTextEdit::textChanged, this, &TextEdit::sendButtonShow);
    sendImagesPath = this->appContext->imagesDir() + "/send.png";
    sendHoverImagesPath = this->appContext->imagesDir() + "/send_hover.png";
    sendDisableImagesPath = this->appContext->imagesDir() + "/send_disable.png";
    sendButton->setStyleSheet(QString(R"(
        QPushButton{
            border: none;
            image: url("%1");
        }
    )")
                                      .arg(sendImagesPath));
    setMouseTracking(true);

    // 样式表（含滚动条宽度）与各部件尺寸统一在 applyDpiScale 中按所在屏 DPI
    // 换算：构造时屏可能仍为主屏，窗口被系统放到其他 DPI 的屏上时由
    // MainWindow 的 DPI 缩放链与换屏兜底再次校正
    applyDpiScale(0);
}

TextEdit::~TextEdit() { }

QSize TextEdit::sizeHint() const
{
    // 高度用项目约定的 190（宽度沿用基类）：TextEditFull 的 sizeHint 与
    // MainWindow 的 pane 高度基准都以 190 + 2 * 15 = 220 为准
    return QSize(QAbstractScrollArea::sizeHint().width(), kBaseTextHeight);
}

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
    QString action1ImagesPath = appContext->imagesDir() + "/cut.png";
    action1->setIcon(QIcon(action1ImagesPath));
    connect(action1, &QAction::triggered, this, &QTextEdit::cut);

    QAction *action2 = menu->addAction("复制");
    action2->setShortcut(QKeySequence("Ctrl+c"));
    QString action2ImagesPath = appContext->imagesDir() + "/menu_copy.png";
    action2->setIcon(QIcon(action2ImagesPath));
    connect(action2, &QAction::triggered, this, &QTextEdit::copy);

    QAction *action3 = menu->addAction("粘贴");
    action3->setShortcut(QKeySequence("Ctrl+v"));
    QString action3ImagesPath = appContext->imagesDir() + "/paste.png";
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

void TextEdit::applyDpiScale(qreal dpi)
{
    // 未指定 DPI 时取控件当前所在屏：跨屏后调用即可自动适配
    if (dpi <= 0) {
        const QScreen *curScreen = screen();
        dpi = curScreen ? curScreen->logicalDotsPerInch() : qreal(DpiUtil::kBaseDpi);
    }
    if (dpi <= 0)
        dpi = qreal(DpiUtil::kBaseDpi);
    const int dpiKey = qRound(dpi);
    const int fontPixelSize = appContext->windowFontPixelSize();
    // 同一 DPI 且字号未变时直接返回：重建样式表会再次触发布局（DPI 缩放链
    // 与主窗口重排都会调用本方法）；字号变化时必须继续（换屏自愈先按新 DPI
    // 用旧字号换算过，MainWindow 随后才更新字号）
    if (dpiKey == appliedDpi && fontPixelSize == appliedFontPixelSize)
        return;
    appliedDpi = dpiKey;
    appliedFontPixelSize = fontPixelSize;

    // 发送按钮的尺寸与图标随比例缩放，物理大小与 144 DPI 屏一致
    const QSize buttonSize(DpiUtil::scalePx(kBaseSendButtonSize, dpi),
                           DpiUtil::scalePx(kBaseSendButtonSize, dpi));
    sendButton->setFixedSize(buttonSize);
    sendButton->setIconSize(buttonSize);

    // 内部布局边距参与缩放：右边距是发送按钮与文本区之间的间隙，其余边距
    // 决定文本与输入框边缘的距离（chatInput 自身的边缘宽度由 TextEditFull
    // 单独换算，见 TextEditFull::applyDpiScale）
    gLayout->setContentsMargins(DpiUtil::scalePx(kBaseGLayoutLeft, dpi, 0),
                                DpiUtil::scalePx(kBaseGLayoutTop, dpi, 0),
                                DpiUtil::scalePx(kBaseGLayoutRight, dpi, 0),
                                DpiUtil::scalePx(kBaseGLayoutBottom, dpi, 0));

    // 滚动条宽度写在样式表里，样式表的 px 不随屏 DPI 变化，需按比例重建
    applyStyleSheet(DpiUtil::scalePx(kBaseScrollBarWidth, dpi));

    // 输入框字体按新像素字号重设（像素字号不随屏 DPI 自动变化，需显式重设）
    if (!font.family().isEmpty()) {
        font.setPixelSize(fontPixelSize);
        setFont(font);
    }
    // 占位符字体同样要参与 DPI 缩放：QTextEdit 内置占位符的字符格式在
    // setPlaceholderText 时按当时的控件字体生成，之后只 setFont 不会重建它
    // （跨屏后占位符仍是构造时的像素字号）。先清空再设回，强制重建格式；
    // setPlaceholderText 对相同文本会提前返回，必须分两次调用
    if (!placeholderText().isEmpty()) {
        const QString placeholder = placeholderText();
        setPlaceholderText(QString());
        setPlaceholderText(placeholder);
    }

    qDebug().nospace().noquote() << "TextEdit 部件尺寸 scale=" << DpiUtil::scaleOf(dpi) << " 按钮="
                                 << buttonSize.width() << "x" << buttonSize.height()
                                 << " 滚动条=" << DpiUtil::scalePx(kBaseScrollBarWidth, dpi)
                                 << " 占位符字号=" << fontPixelSize;
}

void TextEdit::applyStyleSheet(int scrollBarWidth)
{
    // 字体用像素字号（构造函数中 setFont）；QSS 若写 font-size 会覆盖 QFont 设置，
    // 且 pt 字号会被 Qt 按屏 DPI 重解析，跨屏后与全局像素字号不一致
    setStyleSheet(QString(R"(
        QTextEdit{
            border: none;
            background: transparent;
        }
        QScrollBar{
            width: %1px;
        }
    )")
                          .arg(scrollBarWidth));
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
