#include "thinkingbutton.h"

#include <QtGui/QCursor>
#include <QtGui/QPixmap>
#include <QtMath>

ThinkingButton::ThinkingButton(QWidget *parent)
    : QWidget{ parent },
      isShowThinkContent(true),
      backgroundColor(QColor(248, 248, 248)),
      thinkTimeLength(0)
{
    setCursor(Qt::PointingHandCursor);
    startThinkTime = QTime::currentTime();

    thinkingIconPath = imagesDir + "/thinking_icon.png";
    arrowUpPath = imagesDir + "/arrow_up.png";
    arrowDownPath = imagesDir + "/arrow_down.png";

    initUI();
}

ThinkingButton::~ThinkingButton() { }

void ThinkingButton::initUI()
{
    textLabel = new QLabel(tr("思考中..."));
    textLabel->setTextFormat(Qt::PlainText);
    textLabel->setMargin(0);
    textLabel->setIndent(0);
    textLabel->setStyleSheet("QLabel{ padding: 0px; margin: 0px; }");

    int fontId = QFontDatabase::addApplicationFont(fontFilePath);
    if (fontId != -1) {
        QStringList families = QFontDatabase::applicationFontFamilies(fontId);
        if (!families.isEmpty()) {
            QFont font(families.at(0));
            font.setPixelSize(windowFontPixelSize);
            textLabel->setFont(font);
        }
    }
    textLabel->adjustSize();

    int iconSize = textLabel->height() - 6;

    leftIconLabel = new QLabel;
    leftIconLabel->setFixedSize(iconSize, iconSize);
    leftIconLabel->setPixmap(
            QPixmap(thinkingIconPath)
                    .scaled(iconSize, iconSize, Qt::KeepAspectRatio, Qt::SmoothTransformation));

    rightIconLabel = new QLabel;
    rightIconLabel->setFixedSize(iconSize, iconSize);
    rightIconLabel->setPixmap(
            QPixmap(arrowUpPath)
                    .scaled(iconSize, iconSize, Qt::KeepAspectRatio, Qt::SmoothTransformation));

    QHBoxLayout *mainLayout = new QHBoxLayout(this);
    mainLayout->addWidget(leftIconLabel);
    mainLayout->addWidget(textLabel);
    mainLayout->addWidget(rightIconLabel);
    mainLayout->setContentsMargins(5, 5, 5, 5);
    mainLayout->setSpacing(0);

    updateSize();
}

void ThinkingButton::updateSize()
{
    int w = leftIconLabel->width() + textLabel->width() + rightIconLabel->width() + 10;
    int h = textLabel->height() + 10;
    setFixedSize(w, h);
}

void ThinkingButton::enterEvent(QEnterEvent *)
{
    backgroundColor = QColor(232, 232, 232);
    update();
}

void ThinkingButton::leaveEvent(QEvent *)
{
    backgroundColor = QColor(248, 248, 248);
    update();
}

void ThinkingButton::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    QPainterPath path;
    path.addRoundedRect(rect(), 10, 10);

    painter.setPen(Qt::NoPen);
    painter.setBrush(QBrush(backgroundColor));
    painter.drawPath(path.simplified());

    painter.end();
}

void ThinkingButton::mousePressEvent(QMouseEvent *e)
{
    if (e->button() == Qt::LeftButton) {
        isShowThinkContent = !isShowThinkContent;

        int iconSize = textLabel->height() - 6;
        QString path = isShowThinkContent ? arrowUpPath : arrowDownPath;
        rightIconLabel->setPixmap(QPixmap(path).scaled(iconSize, iconSize, Qt::KeepAspectRatio,
                                                       Qt::SmoothTransformation));
        emit clicked();
    }
    QWidget::mousePressEvent(e);
}

void ThinkingButton::setIsShowThinkContent(bool show)
{
    isShowThinkContent = show;
    int iconSize = textLabel->height() - 6;
    QString path = show ? arrowUpPath : arrowDownPath;
    rightIconLabel->setPixmap(QPixmap(path).scaled(iconSize, iconSize, Qt::KeepAspectRatio,
                                                   Qt::SmoothTransformation));
}

void ThinkingButton::setThinkEnd()
{
    QTime end = QTime::currentTime();
    thinkTimeLength = startThinkTime.secsTo(end);
    textLabel->setText(QString(tr("已深度思考(用时%1秒)")).arg(thinkTimeLength));
    textLabel->adjustSize();
    updateSize();
}

void ThinkingButton::setThinkTimeLength(int secs)
{
    thinkTimeLength = secs;
    textLabel->setText(QString(tr("已深度思考(用时%1秒)")).arg(thinkTimeLength));
    textLabel->adjustSize();
    updateSize();
}

int ThinkingButton::getThinkTimeLength()
{
    return thinkTimeLength;
}
