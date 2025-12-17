#ifndef TEXTBOXWIDGET_H
#define TEXTBOXWIDGET_H

#include <QtWidgets/QWidget>

class TextBoxWidget : public QWidget
{
    Q_OBJECT
public:
    explicit TextBoxWidget(QWidget *parent = nullptr);
    ~TextBoxWidget();
};

#endif // TEXTBOXWIDGET_H
