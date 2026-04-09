#ifndef SETTINGWIDGET_H
#define SETTINGWIDGET_H

#pragma once
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
    explicit SettingWidget(QWidget *parent = nullptr);
    ~SettingWidget();

signals:
    void baseUrlTextChanged(const QString &);
    void apiKeyTextChanged(const QString &);
    void modelNameTextChanged(const QString &);
    void maxTokensBoxValueChanged(int);
    void topPBoxValueChanged(double);
    void temperatureBoxValueChanged(double);
    void maxTokensSliderValueChanged(int);
    void topPSliderValueChanged(int);
    void temperatureSliderValueChanged(int);

protected:
    void paintEvent(QPaintEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;

private:
    void loadConfig();
    void setupUI();
    void setupConnections();
    void createModelSelectWidget();
    void createMaxTokensWidget();
    void createTopPWidget();
    void createTemperatureWidget();

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
