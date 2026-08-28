#ifndef IMAGELABEL_H
#define IMAGELABEL_H

#pragma once
#include <QtWidgets/QLabel>

// extern const QString imagesDir;

class AppContext;

class ImageLabel : public QLabel
{
    Q_OBJECT
public:
    explicit ImageLabel(AppContext *appContext, bool isUser = true, QWidget *parent = nullptr);

private:
    AppContext *appContext;
};

#endif // IMAGELABEL_H
