#include "settingwidget.h"
#include "appcontext.h"

SettingWidget::SettingWidget(AppContext *appContext, QWidget *parent)
    : QWidget(parent), appContext(appContext),
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
    QFile configFile(this->appContext->configFilePath());

    if (!QFile::exists(this->appContext->configFilePath())) {
        if (configFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
            QTextStream out(&configFile);
            out.setEncoding(QStringConverter::Utf8);
            out << this->appContext->initBaseUrl() << '\n'
                << this->appContext->initApiKey() << '\n'
                << this->appContext->initModel() << '\n'
                << this->appContext->initMaxTokensCurrentVal() << '\n'
                << this->appContext->initTopPCurrentVal() << '\n'
                << this->appContext->initTemperatureCurrentVal() << '\n';
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
    baseUrlLabel = new Label(this->appContext);
    apiKeyLabel = new Label(this->appContext);
    modelNameLabel = new Label(this->appContext);
    maxTokensLabel = new Label(this->appContext);
    topPLabel = new Label(this->appContext);
    temperatureLabel = new Label(this->appContext);

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

    baseUrlEdit = new SettingEdit(this->appContext);
    apiKeyEdit = new SettingEdit(this->appContext);
    modelNameEdit = new SettingEdit(this->appContext);

    baseUrlEdit->setText(baseUrl);
    apiKeyEdit->setText(apiKey);
    modelNameEdit->setText(model);

    maxTokensBox = new SpinBox(this->appContext);
    topPBox = new DoubleSpinBox(this->appContext);
    temperatureBox = new DoubleSpinBox(this->appContext);

    maxTokensBox->setRange(this->appContext->maxTokensMinimum(), this->appContext->maxTokensMaximum());
    maxTokensBox->setValue(maxTokensCurrentVal);

    topPBox->setRange(this->appContext->topPMinimum(), this->appContext->topPMaximum());
    topPBox->setValue(topPCurrentVal);
    topPBox->setSingleStep(this->appContext->topPSingleStep());

    temperatureBox->setRange(this->appContext->temperatureMinimum(), this->appContext->temperatureMaximum());
    temperatureBox->setValue(temperatureCurrentVal);
    temperatureBox->setSingleStep(this->appContext->temperatureSingleStep());

    maxTokensSlider = new Slider();
    topPSlider = new Slider();
    temperatureSlider = new Slider();

    maxTokensSlider->setMinimum(this->appContext->maxTokensMinimum());
    maxTokensSlider->setMaximum(this->appContext->maxTokensMaximum());
    maxTokensSlider->setValue(maxTokensCurrentVal);

    topPSlider->setMinimum(static_cast<int>(this->appContext->topPMinimum() * 100));
    topPSlider->setMaximum(static_cast<int>(this->appContext->topPMaximum() * 100));
    topPSlider->setValue(static_cast<int>(topPCurrentVal * 100));

    temperatureSlider->setMinimum(static_cast<int>((this->appContext->temperatureMinimum() - 0.01) * 100));
    temperatureSlider->setMaximum(static_cast<int>((this->appContext->temperatureMaximum() - 0.01) * 100));
    temperatureSlider->setValue(static_cast<int>((temperatureCurrentVal - 0.01) * 100));

    createModelSelectWidget();
    createMaxTokensWidget();
    createTopPWidget();
    createTemperatureWidget();

    resize(400, 760);
    setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);

    settingVLayout = new QVBoxLayout(this);
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

    baseUrlWidget = new QWidget();
    baseUrlWidget->setFixedSize(340, 40);
    baseUrlWidget->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    baseUrlWidget->setObjectName("baseUrlWidget");
    baseUrlWidget->setStyleSheet("QWidget#baseUrlWidget{"
                                 "    background: transparent;"
                                 "}");

    baseUrlHLayout = new QHBoxLayout(baseUrlWidget);
    baseUrlHLayout->setSpacing(9);
    baseUrlEdit->setFixedSize(baseUrlWidget->width() - baseUrlLabel->width() - 10,
                              baseUrlEdit->height());
    baseUrlHLayout->addWidget(baseUrlLabel);
    baseUrlHLayout->addWidget(baseUrlEdit);
    baseUrlHLayout->setContentsMargins(0, 4, 0, 4);

    apiKeyWidget = new QWidget();
    apiKeyWidget->setFixedSize(340, 40);
    apiKeyWidget->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    apiKeyWidget->setObjectName("apiKeyWidget");
    apiKeyWidget->setStyleSheet("QWidget#apiKeyWidget{"
                                "    background: transparent;"
                                "}");

    apiKeyHLayout = new QHBoxLayout(apiKeyWidget);
    apiKeyHLayout->setSpacing(9);
    apiKeyEdit->setFixedSize(apiKeyWidget->width() - apiKeyLabel->width() - 10,
                             apiKeyEdit->height());
    apiKeyHLayout->addWidget(apiKeyLabel);
    apiKeyHLayout->addWidget(apiKeyEdit);
    apiKeyHLayout->setContentsMargins(0, 4, 0, 4);

    modelNameWidget = new QWidget();
    modelNameWidget->setFixedSize(340, 40);
    modelNameWidget->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    modelNameWidget->setObjectName("modelNameWidget");
    modelNameWidget->setStyleSheet("QWidget#modelNameWidget{"
                                   "    background: transparent;"
                                   "}");

    modelNameHLayout = new QHBoxLayout(modelNameWidget);
    modelNameHLayout->setSpacing(9);
    modelNameEdit->setFixedSize(modelNameWidget->width() - modelNameLabel->width() - 10,
                                modelNameEdit->height());
    modelNameHLayout->addWidget(modelNameLabel);
    modelNameHLayout->addWidget(modelNameEdit);
    modelNameHLayout->setContentsMargins(0, 4, 0, 4);

    modelSelectVLayout = new QVBoxLayout(modelSelectWidget);
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

    maxTokensTopSubWidget = new QWidget();
    maxTokensTopSubWidget->resize(340, 40);
    maxTokensTopSubWidget->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    maxTokensTopSubWidget->setObjectName("maxTokensTopSubWidget");
    maxTokensTopSubWidget->setStyleSheet("QWidget#maxTokensTopSubWidget{"
                                         "    background: transparent;"
                                         "}");

    maxTokensTopSubHLayout = new QHBoxLayout(maxTokensTopSubWidget);
    maxTokensTopSubHLayout->setSpacing(maxTokensTopSubWidget->width() / 2
                                       - maxTokensLabel->width());
    maxTokensTopSubHLayout->addWidget(maxTokensLabel);
    maxTokensTopSubHLayout->addWidget(maxTokensBox);
    maxTokensTopSubHLayout->setContentsMargins(0, 5, 0, 3);

    maxTokensBottomSubWidget = new QWidget();
    maxTokensBottomSubWidget->resize(340, 40);
    maxTokensBottomSubWidget->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    maxTokensBottomSubWidget->setObjectName("maxTokensBottomSubWidget");
    maxTokensBottomSubWidget->setStyleSheet("QWidget#maxTokensBottomSubWidget{"
                                            "    background: transparent;"
                                            "}");

    maxTokensBottomSubHLayout = new QHBoxLayout(maxTokensBottomSubWidget);
    maxTokensBottomSubHLayout->addWidget(maxTokensSlider);
    maxTokensBottomSubHLayout->setContentsMargins(0, 9, 0, 5);

    maxTokensVLayout = new QVBoxLayout(maxTokensWidget);
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

    topPTopSubWidget = new QWidget();
    topPTopSubWidget->resize(340, 40);
    topPTopSubWidget->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    topPTopSubWidget->setObjectName("topPTopSubWidget");
    topPTopSubWidget->setStyleSheet("QWidget#topPTopSubWidget{"
                                    "    background: transparent;"
                                    "}");

    topPTopSubHLayout = new QHBoxLayout(topPTopSubWidget);
    topPTopSubHLayout->setSpacing(topPTopSubWidget->width() / 2 - topPLabel->width());
    topPTopSubHLayout->addWidget(topPLabel);
    topPTopSubHLayout->addWidget(topPBox);
    topPTopSubHLayout->setContentsMargins(0, 5, 0, 3);

    topPBottomSubWidget = new QWidget();
    topPBottomSubWidget->resize(340, 40);
    topPBottomSubWidget->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    topPBottomSubWidget->setObjectName("topPBottomSubWidget");
    topPBottomSubWidget->setStyleSheet("QWidget#topPBottomSubWidget{"
                                       "    background: transparent;"
                                       "}");

    topPBottomSubHLayout = new QHBoxLayout(topPBottomSubWidget);
    topPBottomSubHLayout->addWidget(topPSlider);
    topPBottomSubHLayout->setContentsMargins(0, 9, 0, 5);

    topPVLayout = new QVBoxLayout(topPWidget);
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

    temperatureTopSubWidget = new QWidget();
    temperatureTopSubWidget->resize(340, 40);
    temperatureTopSubWidget->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    temperatureTopSubWidget->setObjectName("temperatureTopSubWidget");
    temperatureTopSubWidget->setStyleSheet("QWidget#temperatureTopSubWidget{"
                                           "    background: transparent;"
                                           "}");

    temperatureTopSubHLayout = new QHBoxLayout(temperatureTopSubWidget);
    temperatureTopSubHLayout->setSpacing(temperatureTopSubWidget->width() / 2
                                         - temperatureLabel->width());
    temperatureTopSubHLayout->addWidget(temperatureLabel);
    temperatureTopSubHLayout->addWidget(temperatureBox);
    temperatureTopSubHLayout->setContentsMargins(0, 5, 0, 3);

    temperatureBottomSubWidget = new QWidget();
    temperatureBottomSubWidget->resize(340, 40);
    temperatureBottomSubWidget->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    temperatureBottomSubWidget->setObjectName("temperatureBottomSubWidget");
    temperatureBottomSubWidget->setStyleSheet("QWidget#temperatureBottomSubWidget{"
                                              "    background: transparent;"
                                              "}");

    temperatureBottomSubHLayout = new QHBoxLayout(temperatureBottomSubWidget);
    temperatureBottomSubHLayout->addWidget(temperatureSlider);
    temperatureBottomSubHLayout->setContentsMargins(0, 9, 0, 5);

    temperatureVLayout = new QVBoxLayout(temperatureWidget);
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

void SettingWidget::setSize()
{
    modelSelectWidget->setFixedSize(
            width() - settingVLayout->contentsMargins().left()
                    - settingVLayout->contentsMargins().right(),
            qRound((height() - settingVLayout->contentsMargins().top()
                    - settingVLayout->contentsMargins().bottom() - 3 * settingVLayout->spacing())
                   * 190 / qreal(580)));
    maxTokensWidget->setFixedSize(
            width() - settingVLayout->contentsMargins().left()
                    - settingVLayout->contentsMargins().right(),
            qRound((height() - settingVLayout->contentsMargins().top()
                    - settingVLayout->contentsMargins().bottom() - 3 * settingVLayout->spacing())
                   * 130 / qreal(580)));
    topPWidget->setFixedSize(
            width() - settingVLayout->contentsMargins().left()
                    - settingVLayout->contentsMargins().right(),
            qRound((height() - settingVLayout->contentsMargins().top()
                    - settingVLayout->contentsMargins().bottom() - 3 * settingVLayout->spacing())
                   * 130 / qreal(580)));
    temperatureWidget->setFixedSize(
            width() - settingVLayout->contentsMargins().left()
                    - settingVLayout->contentsMargins().right(),
            qRound((height() - settingVLayout->contentsMargins().top()
                    - settingVLayout->contentsMargins().bottom() - 3 * settingVLayout->spacing())
                   * 130 / qreal(580)));

    baseUrlWidget->setFixedSize(
            modelSelectWidget->width() - modelSelectVLayout->contentsMargins().left()
                    - modelSelectVLayout->contentsMargins().right(),
            qRound((modelSelectWidget->height() - modelSelectVLayout->contentsMargins().top()
                    - modelSelectVLayout->contentsMargins().bottom()
                    - 2 * modelSelectVLayout->spacing())
                   / qreal(3)));
    apiKeyWidget->setFixedSize(
            modelSelectWidget->width() - modelSelectVLayout->contentsMargins().left()
                    - modelSelectVLayout->contentsMargins().right(),
            qRound((modelSelectWidget->height() - modelSelectVLayout->contentsMargins().top()
                    - modelSelectVLayout->contentsMargins().bottom()
                    - 2 * modelSelectVLayout->spacing())
                   / qreal(3)));
    modelNameWidget->setFixedSize(
            modelSelectWidget->width() - modelSelectVLayout->contentsMargins().left()
                    - modelSelectVLayout->contentsMargins().right(),
            qRound((modelSelectWidget->height() - modelSelectVLayout->contentsMargins().top()
                    - modelSelectVLayout->contentsMargins().bottom()
                    - 2 * modelSelectVLayout->spacing())
                   / qreal(3)));

    baseUrlLabel->setFixedSize(baseUrlLabel->size());
    apiKeyLabel->setFixedSize(apiKeyLabel->size());
    modelNameLabel->setFixedSize(modelNameLabel->size());
    baseUrlEdit->setFixedSize(baseUrlWidget->width() - baseUrlHLayout->contentsMargins().left()
                                      - baseUrlHLayout->contentsMargins().right()
                                      - baseUrlHLayout->spacing() - baseUrlLabel->width() - 1,
                              baseUrlEdit->height());
    apiKeyEdit->setFixedSize(baseUrlWidget->width() - baseUrlHLayout->contentsMargins().left()
                                     - baseUrlHLayout->contentsMargins().right()
                                     - baseUrlHLayout->spacing() - apiKeyLabel->width() - 1,
                             apiKeyEdit->height());
    modelNameEdit->setFixedSize(baseUrlWidget->width() - baseUrlHLayout->contentsMargins().left()
                                        - baseUrlHLayout->contentsMargins().right()
                                        - baseUrlHLayout->spacing() - modelNameLabel->width() - 1,
                                modelNameEdit->height());

    maxTokensTopSubWidget->setFixedSize(
            maxTokensWidget->width() - maxTokensVLayout->contentsMargins().left()
                    - maxTokensVLayout->contentsMargins().right(),
            qRound((maxTokensWidget->height() - maxTokensVLayout->contentsMargins().top()
                    - maxTokensVLayout->contentsMargins().bottom())
                   / qreal(2)));
    maxTokensBottomSubWidget->setFixedSize(
            maxTokensWidget->width() - maxTokensVLayout->contentsMargins().left()
                    - maxTokensVLayout->contentsMargins().right(),
            qRound((maxTokensWidget->height() - maxTokensVLayout->contentsMargins().top()
                    - maxTokensVLayout->contentsMargins().bottom())
                   / qreal(2)));
    topPTopSubWidget->setFixedSize(
            topPWidget->width() - topPVLayout->contentsMargins().left()
                    - topPVLayout->contentsMargins().right(),
            qRound((topPWidget->height() - topPVLayout->contentsMargins().top()
                    - topPVLayout->contentsMargins().bottom())
                   / qreal(2)));
    topPBottomSubWidget->setFixedSize(
            topPWidget->width() - topPVLayout->contentsMargins().left()
                    - topPVLayout->contentsMargins().right(),
            qRound((topPWidget->height() - topPVLayout->contentsMargins().top()
                    - topPVLayout->contentsMargins().bottom())
                   / qreal(2)));
    temperatureTopSubWidget->setFixedSize(
            temperatureWidget->width() - temperatureVLayout->contentsMargins().left()
                    - temperatureVLayout->contentsMargins().right(),
            qRound((temperatureWidget->height() - temperatureVLayout->contentsMargins().top()
                    - temperatureVLayout->contentsMargins().bottom())
                   / qreal(2)));
    temperatureBottomSubWidget->setFixedSize(
            temperatureWidget->width() - temperatureVLayout->contentsMargins().left()
                    - temperatureVLayout->contentsMargins().right(),
            qRound((temperatureWidget->height() - temperatureVLayout->contentsMargins().top()
                    - temperatureVLayout->contentsMargins().bottom())
                   / qreal(2)));

    maxTokensLabel->setFixedSize(maxTokensLabel->size());
    topPLabel->setFixedSize(topPLabel->size());
    temperatureLabel->setFixedSize(temperatureLabel->size());
    maxTokensTopSubHLayout->setSpacing(maxTokensTopSubWidget->width() / 2
                                       - maxTokensLabel->width());
    topPTopSubHLayout->setSpacing(topPTopSubWidget->width() / 2 - topPLabel->width());
    temperatureTopSubHLayout->setSpacing(temperatureTopSubWidget->width() / 2
                                         - temperatureLabel->width());
    maxTokensBox->setFixedSize(maxTokensTopSubWidget->width() / 2, maxTokensBox->height());
    topPBox->setFixedSize(topPTopSubWidget->width() / 2, topPBox->height());
    temperatureBox->setFixedSize(temperatureTopSubWidget->width() / 2, temperatureBox->height());
    qDebug() << "resizeEvent maxTokensBox size" << maxTokensBox->size()
             << maxTokensTopSubWidget->height() << maxTokensTopSubHLayout->contentsMargins().top()
             << maxTokensTopSubHLayout->contentsMargins().bottom();
    qDebug() << "resizeEvent topPBox size" << topPBox->size() << topPTopSubWidget->height()
             << topPTopSubHLayout->contentsMargins().top()
             << topPTopSubHLayout->contentsMargins().bottom();
    qDebug() << "resizeEvent temperatureBox size" << temperatureBox->size()
             << temperatureTopSubWidget->height()
             << temperatureTopSubHLayout->contentsMargins().top()
             << temperatureTopSubHLayout->contentsMargins().bottom();
    maxTokensSlider->setFixedSize(maxTokensBottomSubWidget->width(), maxTokensSlider->height());
    topPSlider->setFixedSize(topPBottomSubWidget->width(), topPSlider->height());
    temperatureSlider->setFixedSize(temperatureBottomSubWidget->width(),
                                    temperatureSlider->height());
    qDebug() << "resizeEvent maxTokensSlider size" << maxTokensSlider->size()
             << maxTokensBottomSubWidget->height()
             << maxTokensBottomSubHLayout->contentsMargins().top()
             << maxTokensBottomSubHLayout->contentsMargins().bottom();
    qDebug() << "resizeEvent topPSlider size" << topPSlider->size() << topPBottomSubWidget->height()
             << topPBottomSubHLayout->contentsMargins().top()
             << topPBottomSubHLayout->contentsMargins().bottom();
    qDebug() << "resizeEvent temperatureSlider size" << temperatureSlider->size()
             << temperatureBottomSubWidget->height()
             << temperatureBottomSubHLayout->contentsMargins().top()
             << temperatureBottomSubHLayout->contentsMargins().bottom();
}

void SettingWidget::saveWidgetSize()
{
    widgetSizeDict["baseUrlWidget"] = baseUrlWidget->size();
    widgetSizeDict["baseUrlLabel"] = baseUrlLabel->size();
    widgetSizeDict["baseUrlEdit"] = baseUrlEdit->size();
    widgetSizeDict["baseUrlHLayout contentsMargins"] =
            QVariant::fromValue(baseUrlHLayout->contentsMargins());
    widgetSizeDict["baseUrlHLayout spacing"] = baseUrlHLayout->spacing();
    widgetSizeDict["apiKeyWidget"] = apiKeyWidget->size();
    widgetSizeDict["apiKeyLabel"] = apiKeyLabel->size();
    widgetSizeDict["apiKeyEdit"] = apiKeyEdit->size();
    widgetSizeDict["apiKeyHLayout contentsMargins"] =
            QVariant::fromValue(apiKeyHLayout->contentsMargins());
    widgetSizeDict["apiKeyHLayout spacing"] = apiKeyHLayout->spacing();
    widgetSizeDict["modelNameWidget"] = modelNameWidget->size();
    widgetSizeDict["modelNameLabel"] = modelNameLabel->size();
    widgetSizeDict["modelNameEdit"] = modelNameEdit->size();
    widgetSizeDict["modelNameHLayout contentsMargins"] =
            QVariant::fromValue(modelNameHLayout->contentsMargins());
    widgetSizeDict["modelNameHLayout spacing"] = modelNameHLayout->spacing();
    widgetSizeDict["modelSelectWidget"] = modelSelectWidget->size();
    widgetSizeDict["modelSelectVLayout contentsMargins"] =
            QVariant::fromValue(modelSelectVLayout->contentsMargins());
    widgetSizeDict["modelSelectVLayout spacing"] = modelSelectVLayout->spacing();
    widgetSizeDict["maxTokensWidget"] = maxTokensWidget->size();
    widgetSizeDict["maxTokensVLayout contentsMargins"] =
            QVariant::fromValue(maxTokensVLayout->contentsMargins());
    widgetSizeDict["maxTokensTopSubWidget"] = maxTokensTopSubWidget->size();
    widgetSizeDict["maxTokensTopSubHLayout contentsMargins"] =
            QVariant::fromValue(maxTokensTopSubHLayout->contentsMargins());
    widgetSizeDict["maxTokensTopSubHLayout spacing"] = maxTokensTopSubHLayout->spacing();
    widgetSizeDict["maxTokensLabel"] = maxTokensLabel->size();
    widgetSizeDict["maxTokensBox"] = maxTokensBox->size();
    widgetSizeDict["maxTokensBottomSubWidget"] = maxTokensBottomSubWidget->size();
    widgetSizeDict["maxTokensBottomSubHLayout contentsMargins"] =
            QVariant::fromValue(maxTokensBottomSubHLayout->contentsMargins());
    widgetSizeDict["maxTokensSlider"] = maxTokensSlider->size();
    widgetSizeDict["topPWidget"] = topPWidget->size();
    widgetSizeDict["topPVLayout contentsMargins"] =
            QVariant::fromValue(topPVLayout->contentsMargins());
    widgetSizeDict["topPTopSubWidget"] = topPTopSubWidget->size();
    widgetSizeDict["topPTopSubHLayout contentsMargins"] =
            QVariant::fromValue(topPTopSubHLayout->contentsMargins());
    widgetSizeDict["topPTopSubHLayout spacing"] = topPTopSubHLayout->spacing();
    widgetSizeDict["topPLabel"] = topPLabel->size();
    widgetSizeDict["topPBox"] = topPBox->size();
    widgetSizeDict["topPBottomSubWidget"] = topPBottomSubWidget->size();
    widgetSizeDict["topPBottomSubHLayout contentsMargins"] =
            QVariant::fromValue(topPBottomSubHLayout->contentsMargins());
    widgetSizeDict["topPSlider"] = topPSlider->size();
    widgetSizeDict["temperatureWidget"] = temperatureWidget->size();
    widgetSizeDict["temperatureVLayout contentsMargins"] =
            QVariant::fromValue(temperatureVLayout->contentsMargins());
    widgetSizeDict["temperatureTopSubWidget"] = temperatureTopSubWidget->size();
    widgetSizeDict["temperatureTopSubHLayout contentsMargins"] =
            QVariant::fromValue(temperatureTopSubHLayout->contentsMargins());
    widgetSizeDict["temperatureTopSubHLayout spacing"] = temperatureTopSubHLayout->spacing();
    widgetSizeDict["temperatureLabel"] = temperatureLabel->size();
    widgetSizeDict["temperatureBox"] = temperatureBox->size();
    widgetSizeDict["temperatureBottomSubWidget"] = temperatureBottomSubWidget->size();
    widgetSizeDict["temperatureBottomSubHLayout contentsMargins"] =
            QVariant::fromValue(temperatureBottomSubHLayout->contentsMargins());
    widgetSizeDict["temperatureSlider"] = temperatureSlider->size();
    widgetSizeDict["settingVLayout contentsMargins"] =
            QVariant::fromValue(settingVLayout->contentsMargins());
    widgetSizeDict["settingVLayout spacing"] = settingVLayout->spacing();
}

void SettingWidget::resetWidgetSize()
{
    setSize();
    saveWidgetSize();
}

void SettingWidget::updateSize(qreal curDpi, qreal lastDpi)
{
    if (lastDpi == 0)
        return;
    qreal ratio = qreal(curDpi / lastDpi);

    auto scale = [=](const QSize &s) -> QSize {
        return QSize(qRound(s.width() * ratio), qRound(s.height() * ratio));
    };
    auto scaleMargins = [=](const QMargins &m) -> QMargins {
        return QMargins(qRound(m.left() * ratio), qRound(m.top() * ratio),
                        qRound(m.right() * ratio), qRound(m.bottom() * ratio));
    };

    baseUrlLabel->setFixedSize(scale(widgetSizeDict["baseUrlLabel"].value<QSize>()));
    apiKeyLabel->setFixedSize(scale(widgetSizeDict["apiKeyLabel"].value<QSize>()));
    modelNameLabel->setFixedSize(scale(widgetSizeDict["modelNameLabel"].value<QSize>()));
    maxTokensLabel->setFixedSize(scale(widgetSizeDict["maxTokensLabel"].value<QSize>()));
    topPLabel->setFixedSize(scale(widgetSizeDict["topPLabel"].value<QSize>()));
    temperatureLabel->setFixedSize(scale(widgetSizeDict["temperatureLabel"].value<QSize>()));
    qDebug() << "baseUrlLabel" << baseUrlLabel->size();
    qDebug() << "apiKeyLabel" << apiKeyLabel->size();
    qDebug() << "modelNameLabel" << modelNameLabel->size();
    qDebug() << "maxTokensLabel" << maxTokensLabel->size();
    qDebug() << "topPLabel" << topPLabel->size();
    qDebug() << "temperatureLabel" << temperatureLabel->size();

    baseUrlEdit->setSize(scale(widgetSizeDict["baseUrlEdit"].value<QSize>()));
    apiKeyEdit->setSize(scale(widgetSizeDict["apiKeyEdit"].value<QSize>()));
    modelNameEdit->setSize(scale(widgetSizeDict["modelNameEdit"].value<QSize>()));

    baseUrlHLayout->setContentsMargins(
            scaleMargins(widgetSizeDict["baseUrlHLayout contentsMargins"].value<QMargins>()));
    apiKeyHLayout->setContentsMargins(
            scaleMargins(widgetSizeDict["apiKeyHLayout contentsMargins"].value<QMargins>()));
    modelNameHLayout->setContentsMargins(
            scaleMargins(widgetSizeDict["modelNameHLayout contentsMargins"].value<QMargins>()));
    baseUrlHLayout->setSpacing(
            qRound(widgetSizeDict["baseUrlHLayout spacing"].value<int>() * ratio));
    apiKeyHLayout->setSpacing(qRound(widgetSizeDict["apiKeyHLayout spacing"].value<int>() * ratio));
    modelNameHLayout->setSpacing(
            qRound(widgetSizeDict["modelNameHLayout spacing"].value<int>() * ratio));

    baseUrlWidget->setFixedSize(scale(widgetSizeDict["baseUrlWidget"].value<QSize>()));
    if (baseUrlWidget->width()
        < baseUrlLabel->width() + baseUrlEdit->width() + baseUrlHLayout->spacing())
        baseUrlHLayout->setSpacing(baseUrlWidget->width() - baseUrlLabel->width()
                                   - baseUrlEdit->width() - 1);
    apiKeyWidget->setFixedSize(scale(widgetSizeDict["apiKeyWidget"].value<QSize>()));
    if (apiKeyWidget->width()
        < apiKeyLabel->width() + apiKeyEdit->width() + apiKeyHLayout->spacing())
        apiKeyHLayout->setSpacing(apiKeyWidget->width() - apiKeyLabel->width() - apiKeyEdit->width()
                                  - 1);
    modelNameWidget->setFixedSize(scale(widgetSizeDict["modelNameWidget"].value<QSize>()));
    if (modelNameWidget->width()
        < modelNameLabel->width() + modelNameEdit->width() + modelNameHLayout->spacing())
        modelNameHLayout->setSpacing(modelNameWidget->width() - modelNameLabel->width()
                                     - modelNameEdit->width() - 1);
    modelSelectVLayout->setContentsMargins(
            scaleMargins(widgetSizeDict["modelSelectVLayout contentsMargins"].value<QMargins>()));
    modelSelectVLayout->setSpacing(
            qRound(widgetSizeDict["modelSelectVLayout spacing"].value<int>() * ratio));
    modelSelectWidget->setFixedSize(scale(widgetSizeDict["modelSelectWidget"].value<QSize>()));

    maxTokensWidget->setFixedSize(scale(widgetSizeDict["maxTokensWidget"].value<QSize>()));
    maxTokensVLayout->setContentsMargins(
            scaleMargins(widgetSizeDict["maxTokensVLayout contentsMargins"].value<QMargins>()));
    topPWidget->setFixedSize(scale(widgetSizeDict["topPWidget"].value<QSize>()));
    topPVLayout->setContentsMargins(
            scaleMargins(widgetSizeDict["topPVLayout contentsMargins"].value<QMargins>()));
    temperatureWidget->setFixedSize(scale(widgetSizeDict["temperatureWidget"].value<QSize>()));
    temperatureVLayout->setContentsMargins(
            scaleMargins(widgetSizeDict["temperatureVLayout contentsMargins"].value<QMargins>()));

    maxTokensTopSubWidget->setFixedSize(
            scale(widgetSizeDict["maxTokensTopSubWidget"].value<QSize>()));
    topPTopSubWidget->setFixedSize(scale(widgetSizeDict["topPTopSubWidget"].value<QSize>()));
    temperatureTopSubWidget->setFixedSize(
            scale(widgetSizeDict["temperatureTopSubWidget"].value<QSize>()));
    maxTokensTopSubHLayout->setContentsMargins(scaleMargins(
            widgetSizeDict["maxTokensTopSubHLayout contentsMargins"].value<QMargins>()));
    topPTopSubHLayout->setContentsMargins(
            scaleMargins(widgetSizeDict["topPTopSubHLayout contentsMargins"].value<QMargins>()));
    temperatureTopSubHLayout->setContentsMargins(scaleMargins(
            widgetSizeDict["temperatureTopSubHLayout contentsMargins"].value<QMargins>()));
    maxTokensTopSubHLayout->setSpacing(maxTokensTopSubWidget->width() / 2
                                       - maxTokensLabel->width());
    topPTopSubHLayout->setSpacing(topPTopSubWidget->width() / 2 - topPLabel->width());
    temperatureTopSubHLayout->setSpacing(temperatureTopSubWidget->width() / 2
                                         - temperatureLabel->width());
    maxTokensBox->setSize(maxTokensTopSubWidget->width() / 2,
                          qRound(widgetSizeDict["maxTokensBox"].value<QSize>().height() * ratio));
    topPBox->setSize(topPTopSubWidget->width() / 2,
                     qRound(widgetSizeDict["topPBox"].value<QSize>().height() * ratio));
    temperatureBox->setSize(
            temperatureTopSubWidget->width() / 2,
            qRound(widgetSizeDict["temperatureBox"].value<QSize>().height() * ratio));

    maxTokensBottomSubWidget->setFixedSize(
            scale(widgetSizeDict["maxTokensBottomSubWidget"].value<QSize>()));
    topPBottomSubWidget->setFixedSize(scale(widgetSizeDict["topPBottomSubWidget"].value<QSize>()));
    temperatureBottomSubWidget->setFixedSize(
            scale(widgetSizeDict["temperatureBottomSubWidget"].value<QSize>()));
    maxTokensBottomSubHLayout->setContentsMargins(scaleMargins(
            widgetSizeDict["maxTokensBottomSubHLayout contentsMargins"].value<QMargins>()));
    topPBottomSubHLayout->setContentsMargins(
            scaleMargins(widgetSizeDict["topPBottomSubHLayout contentsMargins"].value<QMargins>()));
    temperatureBottomSubHLayout->setContentsMargins(scaleMargins(
            widgetSizeDict["temperatureBottomSubHLayout contentsMargins"].value<QMargins>()));
    maxTokensSlider->setSize(scale(widgetSizeDict["maxTokensSlider"].value<QSize>()));
    topPSlider->setSize(scale(widgetSizeDict["topPSlider"].value<QSize>()));
    temperatureSlider->setSize(scale(widgetSizeDict["temperatureSlider"].value<QSize>()));
}

void SettingWidget::updateLayoutMarginsSpacing(qreal curDpi, qreal lastDpi)
{
    if (lastDpi == 0)
        return;
    qreal ratio = qreal(curDpi / lastDpi);
    auto scaleMargins = [=](const QMargins &m) -> QMargins {
        return QMargins(qRound(m.left() * ratio), qRound(m.top() * ratio),
                        qRound(m.right() * ratio), qRound(m.bottom() * ratio));
    };

    settingVLayout->setContentsMargins(
            scaleMargins(widgetSizeDict["settingVLayout contentsMargins"].value<QMargins>()));
    settingVLayout->setSpacing(
            qRound(widgetSizeDict["settingVLayout spacing"].value<int>() * ratio));
    if (height() < modelSelectWidget->height() + maxTokensWidget->height() + topPWidget->height()
                + temperatureWidget->height() + settingVLayout->contentsMargins().top()
                + settingVLayout->contentsMargins().bottom() + 3 * settingVLayout->spacing()) {
        int settingVLayoutVMargins =
                qRound((height()
                        - (modelSelectWidget->height() + maxTokensWidget->height()
                           + topPWidget->height() + temperatureWidget->height()))
                       * 1 / qreal(4));
        int settingVLayoutSpacing =
                qRound((height()
                        - (modelSelectWidget->height() + maxTokensWidget->height()
                           + topPWidget->height() + temperatureWidget->height()))
                       * 1 / qreal(6));
        settingVLayout->setContentsMargins(
                settingVLayout->contentsMargins().left(), settingVLayoutVMargins,
                settingVLayout->contentsMargins().right(), settingVLayoutVMargins);
        settingVLayout->setSpacing(settingVLayoutSpacing);
    }
}
