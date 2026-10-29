#include "printlabel.h"
#include "appcontext.h"

PrintLabel::PrintLabel(const QString &text, AppContext *appContext, QWidget *parent)
    : QWidget(parent),
      appContext(appContext),
      text(text.trimmed()),
      label(new QLabel()),
      fontMetrics(nullptr),
      mainHLayout(new QHBoxLayout()),
      printTimer(new QTimer(this))
{
    // 字体族由 AppContext 统一注册缓存，直接取用（不再逐个 addApplicationFont）；
    // 像素字号与全局一致：pointSize 会被 Qt 按屏 DPI 自动重解析，
    // 跨屏后与像素字号控件不一致
    const QString fontFamily = appContext->fontFamily();
    if (!fontFamily.isEmpty()) {
        font = QFont(fontFamily);
        font.setPixelSize(appContext->windowFontPixelSize());
        font.setBold(true);
        label->setFont(font);
        fontMetrics = new QFontMetrics(font);
    }
    palette = label->palette();
    palette.setColor(QPalette::WindowText, QColor(76, 106, 246));
    label->setPalette(palette);
    label->setAlignment(Qt::AlignCenter);

    connect(printTimer, &QTimer::timeout, this, &PrintLabel::printEnd);

    if (!this->text.isEmpty()) {
        label->setText(this->text);
        label->adjustSize();
    } else {
        if (fontMetrics) {
            label->resize(fontMetrics->height(), fontMetrics->height());
        } else {
            label->resize(20, 20);
        }
    }
    mainHLayout->addWidget(label);
    mainHLayout->setContentsMargins(5, 5, 5, 5);
    setLayout(mainHLayout);
    setFixedSize(label->width() + 10, label->height() + 10);
}

PrintLabel::~PrintLabel()
{
    delete fontMetrics;
}

void PrintLabel::updateSize(qreal curDpi, qreal lastDpi)
{
    Q_UNUSED(curDpi)
    Q_UNUSED(lastDpi)
    // 字号按新像素值重设并重建度量，再按文本重算 label 尺寸（几何效果等同于
    // 按 DPI 比例缩放，且字体与文字始终匹配）
    if (fontMetrics) {
        font.setPixelSize(appContext->windowFontPixelSize());
        label->setFont(font);
        delete fontMetrics;
        fontMetrics = new QFontMetrics(font);
    }
    if (!text.isEmpty()) {
        label->adjustSize();
    } else if (fontMetrics) {
        label->resize(fontMetrics->height(), fontMetrics->height());
    } else {
        label->resize(20, 20);
    }
    setFixedSize(label->width() + 10, label->height() + 10);
}

void PrintLabel::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    QPainterPath path;
    path.addRoundedRect(rect().x(), rect().y(), rect().width(), rect().height(), 13, 13);

    QBrush brush(Qt::SolidPattern);
    brush.setColor(Qt::white);

    painter.setPen(Qt::NoPen);
    painter.setBrush(brush);
    painter.drawPath(path.simplified());
}

void PrintLabel::setText(const QString &text)
{
    this->text = text.trimmed();
    if (!this->text.isEmpty()) {
        label->setText(this->text);
        label->adjustSize();
    } else {
        if (fontMetrics) {
            label->resize(fontMetrics->height(), fontMetrics->height());
        } else {
            label->resize(20, 20);
        }
    }
    setFixedSize(label->width() + 10, label->height() + 10);
}

void PrintLabel::printStart()
{
    show();
    printTimer->start(2000);
}

void PrintLabel::printEnd()
{
    printTimer->stop();
    hide();
}
