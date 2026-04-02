#ifndef LABEL_H
#define LABEL_H

#pragma once
#include <QtWidgets/QLabel>
#include <QtGui/QFont>
#include <QtGui/QFontDatabase>

class Label : public QLabel
{
    Q_OBJECT
public:
    explicit Label(QWidget *parent = nullptr);
    ~Label();
};

#endif // LABEL_H
