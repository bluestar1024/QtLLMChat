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

class AppContext;

class SettingWidget : public QWidget
{
    Q_OBJECT
public:
    explicit SettingWidget(AppContext *appContext, QWidget *parent = nullptr);
    ~SettingWidget();

    void setSize();
    void saveWidgetSize();
    void resetWidgetSize();
    void updateSize(qreal curDpi, qreal lastDpi);
    void updateLayoutMarginsSpacing(qreal curDpi, qreal lastDpi);

protected:
    void paintEvent(QPaintEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;

private slots:
    void onBaseUrlTextChanged(const QString &text);
    void onApiKeyTextChanged(const QString &text);
    void onModelNameTextChanged(const QString &text);
    void onMaxTokensBoxValueChanged(int i);
    void onTopPBoxValueChanged(double d);
    void onTemperatureBoxValueChanged(double d);
    void onMaxTokensSliderValueChanged(int i);
    void onTopPSliderValueChanged(int i);
    void onTemperatureSliderValueChanged(int i);

private:
    AppContext *appContext;

    void loadConfig();
    // 将配置文件中指定行（从 0 开始计数）的内容替换为 value 后写回：行数不足时补空行
    void writeConfigLine(int lineIndex, const QString &value);
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

    QWidget *baseUrlWidget;
    QHBoxLayout *baseUrlHLayout;
    QWidget *apiKeyWidget;
    QHBoxLayout *apiKeyHLayout;
    QWidget *modelNameWidget;
    QHBoxLayout *modelNameHLayout;
    QVBoxLayout *modelSelectVLayout;
    QWidget *modelSelectWidget;

    QWidget *maxTokensTopSubWidget;
    QHBoxLayout *maxTokensTopSubHLayout;
    QWidget *maxTokensBottomSubWidget;
    QHBoxLayout *maxTokensBottomSubHLayout;
    QVBoxLayout *maxTokensVLayout;
    QWidget *maxTokensWidget;

    QWidget *topPTopSubWidget;
    QHBoxLayout *topPTopSubHLayout;
    QWidget *topPBottomSubWidget;
    QHBoxLayout *topPBottomSubHLayout;
    QVBoxLayout *topPVLayout;
    QWidget *topPWidget;

    QWidget *temperatureTopSubWidget;
    QHBoxLayout *temperatureTopSubHLayout;
    QWidget *temperatureBottomSubWidget;
    QHBoxLayout *temperatureBottomSubHLayout;
    QVBoxLayout *temperatureVLayout;
    QWidget *temperatureWidget;

    QVBoxLayout *settingVLayout;

    QMap<QString, QVariant> widgetSizeDict;
};

#endif // SETTINGWIDGET_H
