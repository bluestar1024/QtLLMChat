#ifndef SETTINGWIDGET_H
#define SETTINGWIDGET_H

#pragma once
#include "mainwindow.h"
#include "globalvariables.h"
#include "label.h"
#include "settingedit.h"
#include "spinbox.h"
#include "doublespinbox.h"
#include "slider.h"

#include <QtWidgets/QWidget>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtGui/QPainter>
#include <QtGui/QPainterPath>
#include <QtGui/QBrush>
#include <QtGui/QColor>
#include <QtGui/QMouseEvent>
#include <QtCore/QString>
#include <QtCore/QFile>
#include <QtCore/QTextStream>

class SettingWidget : public QWidget
{
    Q_OBJECT
public:
    explicit SettingWidget(MainWindow *mainWindow, QWidget *parent = nullptr);
    ~SettingWidget();

    void loadConfig();
    void setupUI();
    void setupConnections();

    QWidget *mainWidget() const;

private:
    void paintEvent(QPaintEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;

    void createModelSelectWidget();
    void createMaxTokensWidget();
    void createTopPWidget();
    void createTemperatureWidget();

    MainWindow *mainWindow;

    QString baseUrl;
    QString apiKey;
    QString model;
    int maxTokensCurrentVal;
    double topPCurrentVal;
    double temperatureCurrentVal;

    Label *baseUrlLabel;
    Label *apiKeyLabel;
    Label *modelNameLabel;
    Label *maxTokensLabel;
    Label *topPLabel;
    Label *temperatureLabel;

    SettingEdit *baseUrlEdit;
    SettingEdit *apiKeyEdit;
    SettingEdit *modelNameEdit;

    SpinBox *maxTokensBox;
    DoubleSpinBox *topPBox;
    DoubleSpinBox *temperatureBox;

    Slider *maxTokensSlider;
    Slider *topPSlider;
    Slider *temperatureSlider;

    QWidget *modelSelectWidget;
    QWidget *maxTokensWidget;
    QWidget *topPWidget;
    QWidget *temperatureWidget;
};

#endif // SETTINGWIDGET_H
