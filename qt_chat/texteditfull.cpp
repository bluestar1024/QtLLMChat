#include "texteditfull.h"

TextEditFull::TextEditFull(QWidget* parent)
    : QWidget(parent), backgroundColorIsLight(false)
{
    setMinimumHeight(80);

    textEdit = new TextEdit();
    mainHLayout = new QHBoxLayout(this);
    mainHLayout->addWidget(textEdit);
    mainHLayout->setContentsMargins(15, 15, 15, 15);

    resize(textEdit->width() + 30, textEdit->height() + 30);
    setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);

    BGColor = textEditFullBGColor;
    BColor = textEditFullBTColor;

    AnimationBackgroundColor = new QPropertyAnimation(this, "backgroundColor");
    AnimationBackgroundColor->setDuration(400);
    AnimationBackgroundColor->setEasingCurve(QEasingCurve::OutQuad);

    AnimationBorderColor = new QPropertyAnimation(this, "borderColor");
    AnimationBorderColor->setDuration(400);
    AnimationBorderColor->setEasingCurve(QEasingCurve::OutQuad);

    setMouseTracking(true);
}

TextEditFull::~TextEditFull()
{
}

void TextEditFull::mouseMoveEvent(QMouseEvent* event) {
    QWidget::mouseMoveEvent(event);
    event->ignore();
}

void TextEditFull::paintEvent(QPaintEvent* event) {
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    QPen pen(BColor);
    painter.setPen(pen);

    QBrush brush(Qt::SolidPattern);
    brush.setColor(BGColor);
    painter.setBrush(brush);

    QPainterPath path;
    path.setFillRule(Qt::WindingFill);
    path.addRoundedRect(rect().adjusted(1, 1, -1, -1), 16, 16);
    painter.drawPath(path.simplified());

    painter.end();
}

QColor TextEditFull::getBackgroundColor() const {
    return BGColor;
}

void TextEditFull::setBackgroundColor(const QColor& color) {
    BGColor = color;
    update();
}

QColor TextEditFull::getBorderColor() const {
    return BColor;
}

void TextEditFull::setBorderColor(const QColor& color) {
    BColor = color;
    update();
}

void TextEditFull::backgroundColorShowLight() {
    if (!backgroundColorIsLight) {
        backgroundColorIsLight = true;
        AnimationBackgroundColor->setStartValue(textEditFullBGColor);
        AnimationBackgroundColor->setEndValue(textEditFullBGTColor);
        AnimationBackgroundColor->start();

        AnimationBorderColor->setStartValue(textEditFullBTColor);
        AnimationBorderColor->setEndValue(textEditFullBColor);
        AnimationBorderColor->start();
    }
}

void TextEditFull::backgroundColorShowDark() {
    if (backgroundColorIsLight) {
        backgroundColorIsLight = false;
        AnimationBackgroundColor->setStartValue(textEditFullBGTColor);
        AnimationBackgroundColor->setEndValue(textEditFullBGColor);
        AnimationBackgroundColor->start();

        AnimationBorderColor->setStartValue(textEditFullBColor);
        AnimationBorderColor->setEndValue(textEditFullBTColor);
        AnimationBorderColor->start();
    }
}

void TextEditFull::clearFocus() {
    textEdit->clearFocus();
}

void TextEditFull::updateSendButtonSize(qreal curDpi, qreal lastDpi) {
    textEdit->updateSendButtonSize(curDpi, lastDpi);
}

void TextEditFull::resetWidgetSize() {
    textEdit->resize(width() - 30, height() - 30);
}

QString TextEditFull::toPlainText() const {
    return textEdit->toPlainText();
}

void TextEditFull::clearText() {
    textEdit->clear();
}

void TextEditFull::setSending(bool isSending)
{
    textEdit->setSending(isSending);
}
