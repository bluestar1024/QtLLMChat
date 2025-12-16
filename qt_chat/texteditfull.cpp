#include "texteditfull.h"

TextEditFull::TextEditFull(QWidget *parent) : QWidget(parent), backgroundColorIsLight(false)
{
    setMinimumHeight(80);

    textEdit = new TextEdit();
    mainHLayout = new QHBoxLayout(this);
    mainHLayout->addWidget(textEdit);
    mainHLayout->setContentsMargins(15, 15, 15, 15);

    resize(textEdit->width() + 30, textEdit->height() + 30);
    setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);

    bgColor = textEditFullBGColor;
    bColor = textEditFullBTColor;

    animationBackgroundColor = new QPropertyAnimation(this, "backgroundColor");
    animationBackgroundColor->setDuration(400);
    animationBackgroundColor->setEasingCurve(QEasingCurve::OutQuad);

    animationBorderColor = new QPropertyAnimation(this, "borderColor");
    animationBorderColor->setDuration(400);
    animationBorderColor->setEasingCurve(QEasingCurve::OutQuad);

    setMouseTracking(true);
}

TextEditFull::~TextEditFull() { }

void TextEditFull::mouseMoveEvent(QMouseEvent *e)
{
    QWidget::mouseMoveEvent(e);
    e->ignore();
}

void TextEditFull::paintEvent(QPaintEvent *e)
{
    Q_UNUSED(e);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    QPen pen(bColor);
    painter.setPen(pen);

    QBrush brush(Qt::SolidPattern);
    brush.setColor(bgColor);
    painter.setBrush(brush);

    QPainterPath path;
    path.setFillRule(Qt::WindingFill);
    path.addRoundedRect(rect().adjusted(1, 1, -1, -1), 16, 16);
    painter.drawPath(path.simplified());

    painter.end();
}

QColor TextEditFull::getBackgroundColor() const
{
    return bgColor;
}

void TextEditFull::setBackgroundColor(const QColor &color)
{
    bgColor = color;
    update();
}

QColor TextEditFull::getBorderColor() const
{
    return bColor;
}

void TextEditFull::setBorderColor(const QColor &color)
{
    bColor = color;
    update();
}

void TextEditFull::backgroundColorShowLight()
{
    if (!backgroundColorIsLight) {
        backgroundColorIsLight = true;
        animationBackgroundColor->setStartValue(textEditFullBGColor);
        animationBackgroundColor->setEndValue(textEditFullBGTColor);
        animationBackgroundColor->start();

        animationBorderColor->setStartValue(textEditFullBTColor);
        animationBorderColor->setEndValue(textEditFullBColor);
        animationBorderColor->start();
    }
}

void TextEditFull::backgroundColorShowDark()
{
    if (backgroundColorIsLight) {
        backgroundColorIsLight = false;
        animationBackgroundColor->setStartValue(textEditFullBGTColor);
        animationBackgroundColor->setEndValue(textEditFullBGColor);
        animationBackgroundColor->start();

        animationBorderColor->setStartValue(textEditFullBColor);
        animationBorderColor->setEndValue(textEditFullBTColor);
        animationBorderColor->start();
    }
}

void TextEditFull::clearFocus()
{
    textEdit->clearFocus();
}

void TextEditFull::updateSendButtonSize(qreal curDpi, qreal lastDpi)
{
    textEdit->updateSendButtonSize(curDpi, lastDpi);
}

void TextEditFull::resetWidgetSize()
{
    textEdit->resize(width() - 30, height() - 30);
}

QString TextEditFull::toPlainText() const
{
    return textEdit->toPlainText();
}

void TextEditFull::clearText()
{
    textEdit->clear();
}

void TextEditFull::setSending(bool isSending)
{
    textEdit->setSending(isSending);
}
