#include "settingwidget.h"
#include "globalvariables.h"

SettingWidget::SettingWidget(QWidget *parent)
    : QWidget(parent),
      maxTokensCurrentVal(0),
      topPCurrentVal(0.0),
      temperatureCurrentVal(0.0),
      baseUrlLabel(nullptr),
      apiKeyLabel(nullptr),
      modelNameLabel(nullptr),
      maxTokensLabel(nullptr),
      topPLabel(nullptr),
      temperatureLabel(nullptr),
      baseUrlEdit(nullptr),
      apiKeyEdit(nullptr),
      modelNameEdit(nullptr),
      maxTokensBox(nullptr),
      topPBox(nullptr),
      temperatureBox(nullptr),
      maxTokensSlider(nullptr),
      topPSlider(nullptr),
      temperatureSlider(nullptr),
      modelSelectWidget(nullptr),
      maxTokensWidget(nullptr),
      topPWidget(nullptr),
      temperatureWidget(nullptr)
{
    setMouseTracking(true);
    loadConfig();
    setupUI();
    setupConnections();
}

SettingWidget::~SettingWidget() { }

void SettingWidget::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    QPainterPath path;
    path.setFillRule(Qt::WindingFill);
    path.addRoundedRect(rect().x(), rect().y(), rect().width(), rect().height(), 16, 16);
    path.addRect(rect().x(), rect().y(), 16, 16);
    path.addRect(rect().width() - 16, rect().y(), 16, 16);
    path.addRect(rect().width() - 16, rect().height() - 16, 16, 16);

    QBrush brush(Qt::SolidPattern);
    brush.setColor(QColor(208, 208, 208));

    painter.setPen(Qt::NoPen);
    painter.setBrush(brush);
    painter.drawPath(path.simplified());
}

void SettingWidget::mouseMoveEvent(QMouseEvent *event)
{
    QWidget::mouseMoveEvent(event);
    event->ignore();
}

void SettingWidget::loadConfig()
{
    QFile configFile(configFilePath);

    if (!QFile::exists(configFilePath)) {
        if (configFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
            QTextStream out(&configFile);
            out.setEncoding(QStringConverter::Utf8);
            out << initBaseUrl << '\n'
                << initApiKey << '\n'
                << initModel << '\n'
                << initMaxTokensCurrentVal << '\n'
                << initTopPCurrentVal << '\n'
                << initTemperatureCurrentVal << '\n';
            configFile.close();
        }
    }

    QStringList content;
    if (configFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&configFile);
        in.setEncoding(QStringConverter::Utf8);
        while (!in.atEnd()) {
            content.append(in.readLine());
        }
        configFile.close();
    }

    if (content.size() >= 6) {
        baseUrl = content[0];
        apiKey = content[1];
        model = content[2];
        maxTokensCurrentVal = content[3].toInt();
        topPCurrentVal = content[4].toDouble();
        temperatureCurrentVal = content[5].toDouble();
    }
}

void SettingWidget::setupUI()
{
    baseUrlLabel = new Label();
    apiKeyLabel = new Label();
    modelNameLabel = new Label();
    maxTokensLabel = new Label();
    topPLabel = new Label();
    temperatureLabel = new Label();

    baseUrlLabel->setText("Base Url");
    apiKeyLabel->setText("Api Key");
    modelNameLabel->setText("Model");
    maxTokensLabel->setText("Max Tokens");
    topPLabel->setText("Top P");
    temperatureLabel->setText("Temperature");

    baseUrlLabel->adjustSize();
    baseUrlLabel->setFixedWidth(baseUrlLabel->width() + 2);
    apiKeyLabel->adjustSize();
    apiKeyLabel->setFixedWidth(apiKeyLabel->width() + 2);
    modelNameLabel->adjustSize();
    modelNameLabel->setFixedWidth(modelNameLabel->width() + 2);
    maxTokensLabel->adjustSize();
    topPLabel->adjustSize();
    temperatureLabel->adjustSize();

    baseUrlEdit = new SettingEdit();
    apiKeyEdit = new SettingEdit();
    modelNameEdit = new SettingEdit();

    baseUrlEdit->setText(baseUrl);
    apiKeyEdit->setText(apiKey);
    modelNameEdit->setText(model);

    maxTokensBox = new SpinBox();
    topPBox = new DoubleSpinBox();
    temperatureBox = new DoubleSpinBox();

    maxTokensBox->setRange(maxTokensMinimum, maxTokensMaximum);
    maxTokensBox->setValue(maxTokensCurrentVal);

    topPBox->setRange(topPMinimum, topPMaximum);
    topPBox->setValue(topPCurrentVal);
    topPBox->setSingleStep(topPSingleStep);

    temperatureBox->setRange(temperatureMinimum, temperatureMaximum);
    temperatureBox->setValue(temperatureCurrentVal);
    temperatureBox->setSingleStep(temperatureSingleStep);

    maxTokensSlider = new Slider();
    topPSlider = new Slider();
    temperatureSlider = new Slider();

    maxTokensSlider->setMinimum(maxTokensMinimum);
    maxTokensSlider->setMaximum(maxTokensMaximum);
    maxTokensSlider->setValue(maxTokensCurrentVal);

    topPSlider->setMinimum(static_cast<int>(topPMinimum * 100));
    topPSlider->setMaximum(static_cast<int>(topPMaximum * 100));
    topPSlider->setValue(static_cast<int>(topPCurrentVal * 100));

    temperatureSlider->setMinimum(static_cast<int>((temperatureMinimum - 0.01) * 100));
    temperatureSlider->setMaximum(static_cast<int>((temperatureMaximum - 0.01) * 100));
    temperatureSlider->setValue(static_cast<int>((temperatureCurrentVal - 0.01) * 100));

    createModelSelectWidget();
    createMaxTokensWidget();
    createTopPWidget();
    createTemperatureWidget();

    resize(400, 760);
    setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);

    QVBoxLayout *settingVLayout = new QVBoxLayout(this);
    settingVLayout->addWidget(modelSelectWidget);
    settingVLayout->addWidget(maxTokensWidget);
    settingVLayout->addWidget(topPWidget);
    settingVLayout->addWidget(temperatureWidget);
    settingVLayout->setContentsMargins(15, 45, 15, 45);
    settingVLayout->setSpacing(30);
}

void SettingWidget::setupConnections()
{
    connect(baseUrlEdit, &QLineEdit::textChanged, this, &SettingWidget::baseUrlTextChanged);
    connect(apiKeyEdit, &QLineEdit::textChanged, this, &SettingWidget::apiKeyTextChanged);
    connect(modelNameEdit, &QLineEdit::textChanged, this, &SettingWidget::modelNameTextChanged);

    connect(maxTokensBox, QOverload<int>::of(&QSpinBox::valueChanged), this,
            &SettingWidget::maxTokensBoxValueChanged);
    connect(topPBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this,
            &SettingWidget::topPBoxValueChanged);
    connect(temperatureBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this,
            &SettingWidget::temperatureBoxValueChanged);

    connect(maxTokensSlider, &QSlider::valueChanged, this,
            &SettingWidget::maxTokensSliderValueChanged);
    connect(topPSlider, &QSlider::valueChanged, this, &SettingWidget::topPSliderValueChanged);
    connect(temperatureSlider, &QSlider::valueChanged, this,
            &SettingWidget::temperatureSliderValueChanged);
}

void SettingWidget::createModelSelectWidget()
{
    modelSelectWidget = new QWidget();
    modelSelectWidget->setFixedSize(370, 190);
    modelSelectWidget->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    modelSelectWidget->setObjectName("modelSelectWidget");
    modelSelectWidget->setStyleSheet("QWidget#modelSelectWidget{"
                                     "    border-radius: 15px;"
                                     "    background: white;"
                                     "}");

    QWidget *baseUrlWidget = new QWidget();
    baseUrlWidget->setFixedSize(340, 40);
    baseUrlWidget->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    baseUrlWidget->setObjectName("baseUrlWidget");
    baseUrlWidget->setStyleSheet("QWidget#baseUrlWidget{"
                                 "    background: transparent;"
                                 "}");

    QHBoxLayout *baseUrlHLayout = new QHBoxLayout(baseUrlWidget);
    baseUrlHLayout->setSpacing(9);
    baseUrlEdit->setFixedSize(baseUrlWidget->width() - baseUrlLabel->width() - 10,
                              baseUrlEdit->height());
    baseUrlHLayout->addWidget(baseUrlLabel);
    baseUrlHLayout->addWidget(baseUrlEdit);
    baseUrlHLayout->setContentsMargins(0, 4, 0, 4);

    QWidget *apiKeyWidget = new QWidget();
    apiKeyWidget->setFixedSize(340, 40);
    apiKeyWidget->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    apiKeyWidget->setObjectName("apiKeyWidget");
    apiKeyWidget->setStyleSheet("QWidget#apiKeyWidget{"
                                "    background: transparent;"
                                "}");

    QHBoxLayout *apiKeyHLayout = new QHBoxLayout(apiKeyWidget);
    apiKeyHLayout->setSpacing(9);
    apiKeyEdit->setFixedSize(apiKeyWidget->width() - apiKeyLabel->width() - 10,
                             apiKeyEdit->height());
    apiKeyHLayout->addWidget(apiKeyLabel);
    apiKeyHLayout->addWidget(apiKeyEdit);
    apiKeyHLayout->setContentsMargins(0, 4, 0, 4);

    QWidget *modelNameWidget = new QWidget();
    modelNameWidget->setFixedSize(340, 40);
    modelNameWidget->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    modelNameWidget->setObjectName("modelNameWidget");
    modelNameWidget->setStyleSheet("QWidget#modelNameWidget{"
                                   "    background: transparent;"
                                   "}");

    QHBoxLayout *modelNameHLayout = new QHBoxLayout(modelNameWidget);
    modelNameHLayout->setSpacing(9);
    modelNameEdit->setFixedSize(modelNameWidget->width() - modelNameLabel->width() - 10,
                                modelNameEdit->height());
    modelNameHLayout->addWidget(modelNameLabel);
    modelNameHLayout->addWidget(modelNameEdit);
    modelNameHLayout->setContentsMargins(0, 4, 0, 4);

    QVBoxLayout *modelSelectVLayout = new QVBoxLayout(modelSelectWidget);
    modelSelectVLayout->addWidget(baseUrlWidget);
    modelSelectVLayout->addWidget(apiKeyWidget);
    modelSelectVLayout->addWidget(modelNameWidget);
    modelSelectVLayout->setContentsMargins(15, 20, 15, 20);
    modelSelectVLayout->setSpacing(15);
}

void SettingWidget::createMaxTokensWidget()
{
    maxTokensWidget = new QWidget();
    maxTokensWidget->resize(370, 130);
    maxTokensWidget->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    maxTokensWidget->setObjectName("maxTokensWidget");
    maxTokensWidget->setStyleSheet("QWidget#maxTokensWidget{"
                                   "    border-radius: 15px;"
                                   "    background: white;"
                                   "}");

    QWidget *maxTokensTopSubWidget = new QWidget();
    maxTokensTopSubWidget->resize(340, 40);
    maxTokensTopSubWidget->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    maxTokensTopSubWidget->setObjectName("maxTokensTopSubWidget");
    maxTokensTopSubWidget->setStyleSheet("QWidget#maxTokensTopSubWidget{"
                                         "    background: transparent;"
                                         "}");

    QHBoxLayout *maxTokensTopSubHLayout = new QHBoxLayout(maxTokensTopSubWidget);
    maxTokensTopSubHLayout->setSpacing(maxTokensTopSubWidget->width() / 2
                                       - maxTokensLabel->width());
    maxTokensTopSubHLayout->addWidget(maxTokensLabel);
    maxTokensTopSubHLayout->addWidget(maxTokensBox);
    maxTokensTopSubHLayout->setContentsMargins(0, 5, 0, 3);

    QWidget *maxTokensBottomSubWidget = new QWidget();
    maxTokensBottomSubWidget->resize(340, 40);
    maxTokensBottomSubWidget->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    maxTokensBottomSubWidget->setObjectName("maxTokensBottomSubWidget");
    maxTokensBottomSubWidget->setStyleSheet("QWidget#maxTokensBottomSubWidget{"
                                            "    background: transparent;"
                                            "}");

    QHBoxLayout *maxTokensBottomSubHLayout = new QHBoxLayout(maxTokensBottomSubWidget);
    maxTokensBottomSubHLayout->addWidget(maxTokensSlider);
    maxTokensBottomSubHLayout->setContentsMargins(0, 9, 0, 5);

    QVBoxLayout *maxTokensVLayout = new QVBoxLayout(maxTokensWidget);
    maxTokensVLayout->addWidget(maxTokensTopSubWidget);
    maxTokensVLayout->addWidget(maxTokensBottomSubWidget);
    maxTokensVLayout->setContentsMargins(15, 25, 15, 25);
    maxTokensVLayout->setSpacing(0);
}

void SettingWidget::createTopPWidget()
{
    topPWidget = new QWidget();
    topPWidget->resize(370, 130);
    topPWidget->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    topPWidget->setObjectName("topPWidget");
    topPWidget->setStyleSheet("QWidget#topPWidget{"
                              "    border-radius: 15px;"
                              "    background: white;"
                              "}");

    QWidget *topPTopSubWidget = new QWidget();
    topPTopSubWidget->resize(340, 40);
    topPTopSubWidget->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    topPTopSubWidget->setObjectName("topPTopSubWidget");
    topPTopSubWidget->setStyleSheet("QWidget#topPTopSubWidget{"
                                    "    background: transparent;"
                                    "}");

    QHBoxLayout *topPTopSubHLayout = new QHBoxLayout(topPTopSubWidget);
    topPTopSubHLayout->setSpacing(topPTopSubWidget->width() / 2 - topPLabel->width());
    topPTopSubHLayout->addWidget(topPLabel);
    topPTopSubHLayout->addWidget(topPBox);
    topPTopSubHLayout->setContentsMargins(0, 5, 0, 3);

    QWidget *topPBottomSubWidget = new QWidget();
    topPBottomSubWidget->resize(340, 40);
    topPBottomSubWidget->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    topPBottomSubWidget->setObjectName("topPBottomSubWidget");
    topPBottomSubWidget->setStyleSheet("QWidget#topPBottomSubWidget{"
                                       "    background: transparent;"
                                       "}");

    QHBoxLayout *topPBottomSubHLayout = new QHBoxLayout(topPBottomSubWidget);
    topPBottomSubHLayout->addWidget(topPSlider);
    topPBottomSubHLayout->setContentsMargins(0, 9, 0, 5);

    QVBoxLayout *topPVLayout = new QVBoxLayout(topPWidget);
    topPVLayout->addWidget(topPTopSubWidget);
    topPVLayout->addWidget(topPBottomSubWidget);
    topPVLayout->setContentsMargins(15, 25, 15, 25);
    topPVLayout->setSpacing(0);
}

void SettingWidget::createTemperatureWidget()
{
    temperatureWidget = new QWidget();
    temperatureWidget->resize(370, 130);
    temperatureWidget->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    temperatureWidget->setObjectName("temperatureWidget");
    temperatureWidget->setStyleSheet("QWidget#temperatureWidget{"
                                     "    border-radius: 15px;"
                                     "    background: white;"
                                     "}");

    QWidget *temperatureTopSubWidget = new QWidget();
    temperatureTopSubWidget->resize(340, 40);
    temperatureTopSubWidget->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    temperatureTopSubWidget->setObjectName("temperatureTopSubWidget");
    temperatureTopSubWidget->setStyleSheet("QWidget#temperatureTopSubWidget{"
                                           "    background: transparent;"
                                           "}");

    QHBoxLayout *temperatureTopSubHLayout = new QHBoxLayout(temperatureTopSubWidget);
    temperatureTopSubHLayout->setSpacing(temperatureTopSubWidget->width() / 2
                                         - temperatureLabel->width());
    temperatureTopSubHLayout->addWidget(temperatureLabel);
    temperatureTopSubHLayout->addWidget(temperatureBox);
    temperatureTopSubHLayout->setContentsMargins(0, 5, 0, 3);

    QWidget *temperatureBottomSubWidget = new QWidget();
    temperatureBottomSubWidget->resize(340, 40);
    temperatureBottomSubWidget->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    temperatureBottomSubWidget->setObjectName("temperatureBottomSubWidget");
    temperatureBottomSubWidget->setStyleSheet("QWidget#temperatureBottomSubWidget{"
                                              "    background: transparent;"
                                              "}");

    QHBoxLayout *temperatureBottomSubHLayout = new QHBoxLayout(temperatureBottomSubWidget);
    temperatureBottomSubHLayout->addWidget(temperatureSlider);
    temperatureBottomSubHLayout->setContentsMargins(0, 9, 0, 5);

    QVBoxLayout *temperatureVLayout = new QVBoxLayout(temperatureWidget);
    temperatureVLayout->addWidget(temperatureTopSubWidget);
    temperatureVLayout->addWidget(temperatureBottomSubWidget);
    temperatureVLayout->setContentsMargins(15, 25, 15, 25);
    temperatureVLayout->setSpacing(0);
}

void SettingWidget::maxTokensBoxSetValue(int i)
{
    maxTokensBox->setValue(i);
}

void SettingWidget::topPBoxSetValue(double i)
{
    topPBox->setValue(i);
}

void SettingWidget::temperatureBoxSetValue(double i)
{
    temperatureBox->setValue(i);
}

void SettingWidget::maxTokensSliderSetValue(int i)
{
    maxTokensSlider->setValue(i);
}

void SettingWidget::topPSliderSetValue(int i)
{
    topPSlider->setValue(i);
}

void SettingWidget::temperatureSliderSetValue(int i)
{
    temperatureSlider->setValue(i);
}
