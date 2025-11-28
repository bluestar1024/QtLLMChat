#ifndef IMAGELABEL_H
#define IMAGELABEL_H

#pragma once
#include <QLabel>

class ImageLabel : public QLabel
{
    Q_OBJECT
public:
    explicit ImageLabel(bool isUser = true, QWidget *parent = nullptr);
};

#endif // IMAGELABEL_H
