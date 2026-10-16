#include "custommenu.h"

CustomMenu::CustomMenu(QWidget *parent) : QMenu(parent)
{
    setWindowFlags(Qt::Popup | Qt::FramelessWindowHint | Qt::NoDropShadowWindowHint);
    setAttribute(Qt::WA_TranslucentBackground);
}

CustomMenu::~CustomMenu() { }
