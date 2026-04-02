#ifndef SETTINGEDIT_H
#define SETTINGEDIT_H

#pragma once
#include <QtWidgets/QLineEdit>

class SettingEdit : public QLineEdit
{
    Q_OBJECT
public:
    explicit SettingEdit(QWidget *parent = nullptr);
    ~SettingEdit();

    void setSize(int width, int height);

private:
    void updateStyleSheet();
};

#endif // SETTINGEDIT_H
