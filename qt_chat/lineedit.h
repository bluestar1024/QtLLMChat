#ifndef LINEEDIT_H
#define LINEEDIT_H

#pragma once
#include "pushbutton.h"

#include <QtWidgets/QLineEdit>
#include <QtGui/QFont>
#include <QtGui/QFontDatabase>
#include <QtGui/QIcon>
#include <QtCore/QSize>
#include <QtCore/QMap>

class LineEdit : public QLineEdit
{
    Q_OBJECT
public:
    explicit LineEdit(QWidget *parent = nullptr);
    ~LineEdit();

    void connectSearchButtonClick(QObject *receiver, const char *slot);
    void resetWidgetSize();
    void updateSize(int curDpi, int lastDpi);

private:
    QPushButton *searchButton;
    QFont font;
    QMap<QString, QSize> widgetSizeDict;
};

#endif // LINEEDIT_H
