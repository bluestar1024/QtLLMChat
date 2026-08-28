#ifndef LABEL_H
#define LABEL_H

#pragma once
#include <QtWidgets/QLabel>
#include <QtGui/QFont>
#include <QtGui/QFontDatabase>

class AppContext;

class Label : public QLabel
{
    Q_OBJECT
public:
    explicit Label(AppContext *appContext, QWidget *parent = nullptr);
    ~Label();

private:
    AppContext *appContext;
};

#endif // LABEL_H
