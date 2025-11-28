#include "imagelabel.h"
#include <QPixmap>

ImageLabel::ImageLabel(bool isUser, QWidget *parent)
    : QLabel(parent)
{
    setFixedSize(32, 32);

    const QString userImg = QStringLiteral(":/images/user.png");
    const QString aiImg   = QStringLiteral(":/images/ai.png");

    setPixmap(QPixmap(isUser ? userImg : aiImg));
}
