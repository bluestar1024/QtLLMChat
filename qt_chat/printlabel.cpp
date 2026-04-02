#include "printlabel.h"
#include "globalvariables.h"

PrintLabel::PrintLabel(const QString &text, QWidget *parent)
    : QWidget(parent),
      text(text.trimmed()),
      label(new QLabel()),
      fontMetrics(nullptr),
      mainHLayout(new QHBoxLayout()),
      printTimer(new QTimer(this))
{
    int fontId = QFontDatabase::addApplicationFont(fontFilePath);
    if (fontId != -1) {
        QStringList fontFamilies = QFontDatabase::applicationFontFamilies(fontId);
        if (!fontFamilies.isEmpty()) {
            QString fontFamily = fontFamilies.at(0);
            font = QFont(fontFamily, windowFontPointSize);
            font.setBold(true);
            label->setFont(font);
            fontMetrics = new QFontMetrics(font);
        }
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

void PrintLabel::updateSize(int curDpi, int lastDpi)
{
    label->resize(qRound(label->width() * static_cast<qreal>(curDpi) / lastDpi),
                  qRound(label->height() * static_cast<qreal>(curDpi) / lastDpi));
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
