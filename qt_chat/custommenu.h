#ifndef CUSTOMMENU_H
#define CUSTOMMENU_H

#pragma once
#include <QtWidgets/QMenu>

class CustomMenu : public QMenu
{
    Q_OBJECT
public:
    explicit CustomMenu(QWidget *parent = nullptr);
    ~CustomMenu();
};

#endif // CUSTOMMENU_H
