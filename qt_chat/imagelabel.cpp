#include "imagelabel.h"
#include "appcontext.h"

#include <QPixmap>

ImageLabel::ImageLabel(AppContext *appContext, bool isUser, QWidget *parent)
    : QLabel(parent), appContext(appContext)
{
    setFixedSize(32, 32);

    const QString userImagePath(this->appContext->imagesDir() + "/user.png");
    const QString aiImagePath(this->appContext->imagesDir() + "/ai.png");

    setPixmap(QPixmap(isUser ? userImagePath : aiImagePath));
}
