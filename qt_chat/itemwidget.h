#ifndef ITEMWIDGET_H
#define ITEMWIDGET_H

#include <QtWidgets/QWidget>

class ItemWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ItemWidget(QWidget *parent = nullptr);
    ~ItemWidget();
};

#endif // ITEMWIDGET_H
