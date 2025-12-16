#ifndef CUSTOMMENU_H
#define CUSTOMMENU_H

#pragma once
#include <QtWidgets/QMenu>

class CustomMenu : public QMenu
{
    Q_OBJECT
public:
    explicit CustomMenu(QWidget *parent = nullptr) : QMenu(parent)
    {
        setWindowFlags(Qt::Popup | Qt::FramelessWindowHint | Qt::NoDropShadowWindowHint);
        setAttribute(Qt::WA_TranslucentBackground);
    }
};

#endif // CUSTOMMENU_H
