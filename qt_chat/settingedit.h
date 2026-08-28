#ifndef SETTINGEDIT_H
#define SETTINGEDIT_H

#pragma once
#include <QtWidgets/QLineEdit>

class AppContext;

class SettingEdit : public QLineEdit
{
    Q_OBJECT
public:
    explicit SettingEdit(AppContext *appContext, QWidget *parent = nullptr);
    ~SettingEdit();

    void setSize(QSize s);

private:
    void updateStyleSheet();

    AppContext *appContext;
};

#endif // SETTINGEDIT_H
