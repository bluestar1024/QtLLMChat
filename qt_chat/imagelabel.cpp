#include "imagelabel.h"
#include <QPixmap>

ImageLabel::ImageLabel(bool isUser, QWidget *parent) : QLabel(parent)
{
    setFixedSize(32, 32);

    const QString userImagePath(imagesDir + "/user.png");
    const QString aiImagePath(imagesDir + "/ai.png");

    setPixmap(QPixmap(isUser ? userImagePath : aiImagePath));
}
