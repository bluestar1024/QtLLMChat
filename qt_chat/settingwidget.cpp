#include "settingwidget.h"
#include "appcontext.h"

#include <QtGui/QFontMetrics>

#include <stdexcept>

// 布局日志用：布局的 margin 与 spacing 拼成一行
static QString layoutInfo(const QLayout *layout)
{
    const QMargins margins = layout->contentsMargins();
    return QString("margin=(%1,%2,%3,%4) spacing=%5")
            .arg(margins.left())
            .arg(margins.top())
            .arg(margins.right())
            .arg(margins.bottom())
            .arg(layout->spacing());
}

// 布局日志用：控件宽高
static QString sizeInfo(const QWidget *widget)
{
    return QString("%1x%2").arg(widget->width()).arg(widget->height());
}

SettingWidget::SettingWidget(AppContext *appContext, QWidget *parent)
    : QWidget(parent),
      appContext(appContext),
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

    fitLabelSize(baseUrlLabel);
    fitLabelSize(apiKeyLabel);
    fitLabelSize(modelNameLabel);
    fitLabelSize(maxTokensLabel);
    fitLabelSize(topPLabel);
    fitLabelSize(temperatureLabel);

    baseUrlEdit = new SettingEdit(this->appContext);
    apiKeyEdit = new SettingEdit(this->appContext);
    modelNameEdit = new SettingEdit(this->appContext);

    baseUrlEdit->setText(baseUrl);
    apiKeyEdit->setText(apiKey);
    modelNameEdit->setText(model);

    maxTokensBox = new SpinBox(this->appContext);
    topPBox = new DoubleSpinBox(this->appContext);
    temperatureBox = new DoubleSpinBox(this->appContext);

    maxTokensBox->setRange(this->appContext->maxTokensMinimum(),
                           this->appContext->maxTokensMaximum());
    maxTokensBox->setValue(maxTokensCurrentVal);

    topPBox->setRange(this->appContext->topPMinimum(), this->appContext->topPMaximum());
    topPBox->setValue(topPCurrentVal);
    topPBox->setSingleStep(this->appContext->topPSingleStep());

    temperatureBox->setRange(this->appContext->temperatureMinimum(),
                             this->appContext->temperatureMaximum());
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

    temperatureSlider->setMinimum(
            static_cast<int>((this->appContext->temperatureMinimum() - 0.01) * 100));
    temperatureSlider->setMaximum(
            static_cast<int>((this->appContext->temperatureMaximum() - 0.01) * 100));
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
    connect(baseUrlEdit, &QLineEdit::textChanged, this, &SettingWidget::onBaseUrlTextChanged);
    connect(apiKeyEdit, &QLineEdit::textChanged, this, &SettingWidget::onApiKeyTextChanged);
    connect(modelNameEdit, &QLineEdit::textChanged, this, &SettingWidget::onModelNameTextChanged);

    connect(maxTokensBox, QOverload<int>::of(&QSpinBox::valueChanged), this,
            &SettingWidget::onMaxTokensBoxValueChanged);
    connect(topPBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this,
            &SettingWidget::onTopPBoxValueChanged);
    connect(temperatureBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this,
            &SettingWidget::onTemperatureBoxValueChanged);

    connect(maxTokensSlider, &QSlider::valueChanged, this,
            &SettingWidget::onMaxTokensSliderValueChanged);
    connect(topPSlider, &QSlider::valueChanged, this, &SettingWidget::onTopPSliderValueChanged);
    connect(temperatureSlider, &QSlider::valueChanged, this,
            &SettingWidget::onTemperatureSliderValueChanged);
}

// 将配置文件中指定行（从 0 开始计数）的内容替换为 value 后写回：行数不足时补空行
void SettingWidget::writeConfigLine(int lineIndex, const QString &value)
{
    try {
        QFile file(appContext->configFilePath());
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            throw std::runtime_error("无法打开文件");
        }

        QTextStream in(&file);
        QStringList lines;
        while (!in.atEnd()) {
            lines.append(in.readLine());
        }
        file.close();

        while (lines.size() <= lineIndex) {
            lines.append("");
        }
        lines[lineIndex] = value;

        if (!file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate)) {
            throw std::runtime_error("无法写入文件");
        }

        QTextStream out(&file);
        for (int i = 0; i < lines.size(); ++i) {
            out << lines[i];
            if (i < lines.size() - 1) {
                out << "\n";
            }
        }
        file.close();

    } catch (const std::exception &e) {
        qDebug() << "错误：" << e.what();
    } catch (...) {
        qDebug() << "发生未知错误";
    }
}

void SettingWidget::onBaseUrlTextChanged(const QString &text)
{
    writeConfigLine(0, text);
}

void SettingWidget::onApiKeyTextChanged(const QString &text)
{
    writeConfigLine(1, text);
}

void SettingWidget::onModelNameTextChanged(const QString &text)
{
    writeConfigLine(2, text);
}

void SettingWidget::onMaxTokensBoxValueChanged(int i)
{
    writeConfigLine(3, QString::number(i));
    maxTokensSlider->setValue(i);
}

void SettingWidget::onTopPBoxValueChanged(double d)
{
    writeConfigLine(4, QString::number(d));
    topPSlider->setValue(static_cast<int>(d * 100));
}

void SettingWidget::onTemperatureBoxValueChanged(double d)
{
    writeConfigLine(5, QString::number(d));
    temperatureSlider->setValue(static_cast<int>((d - 0.01) * 100));
}

void SettingWidget::onMaxTokensSliderValueChanged(int i)
{
    writeConfigLine(3, QString::number(i));
    maxTokensBox->setValue(i);
}

void SettingWidget::onTopPSliderValueChanged(int i)
{
    writeConfigLine(4, QString::number(i / 100.0));
    topPBox->setValue(i / 100.0);
}

void SettingWidget::onTemperatureSliderValueChanged(int i)
{
    writeConfigLine(5, QString::number(i / 100.0 + 0.01));
    temperatureBox->setValue(i / 100.0 + 0.01);
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
    // 输入框宽度不再手工拼算（行宽 - 标签宽 - 间距）：交给布局按剩余空间分配，
    // 多轮 DPI 缩放后即使各固定尺寸的取整误差互不抵消，右边缘也不会越出行
    baseUrlHLayout->addWidget(baseUrlLabel);
    baseUrlHLayout->addWidget(baseUrlEdit, 1);
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
    apiKeyHLayout->addWidget(apiKeyLabel);
    apiKeyHLayout->addWidget(apiKeyEdit, 1);
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
    modelNameHLayout->addWidget(modelNameLabel);
    modelNameHLayout->addWidget(modelNameEdit, 1);
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

// 标签宽高按当前字体度量重新拟合：字体像素字号与字形 advance 都是整数，
// 文本总宽与字号并非严格线性。此前宽度取 adjustSize() 结果 + 2 后按 DPI
// 比例逐级缩放，多数缩放比例下小于实际文本宽度，文本右边缘被部件边缘裁掉
void SettingWidget::fitLabelSize(Label *label)
{
    if (!label)
        return;

    // 先解除上一次的定尺寸约束，sizeHint 才能按当前字体重新度量
    label->setMinimumSize(0, 0);
    label->setMaximumSize(QWIDGETSIZE_MAX, QWIDGETSIZE_MAX);
    const QSize hint = label->sizeHint();
    const QFontMetrics fm(label->font());
    // 边框与 QLabel margin 占据绘制区两侧，文本可用宽度 = 总宽 - extra
    const int extra = 2 * label->margin() + 2 * label->frameWidth();
    // 宽度取 sizeHint（含样式 padding）与文本 advance 的较大值，并留 2px 余量
    label->setFixedSize(qMax(hint.width(), fm.horizontalAdvance(label->text()) + extra) + 2,
                        qMax(hint.height(), fm.height() + extra));
}

// 数值行布局：标签宽度随字体度量变化后不能再固定用“行宽 / 2”做间距，
// 否则标签变宽时间距被压成负值，标签与数值框重叠
void SettingWidget::layoutValueRow(QHBoxLayout *layout, QWidget *subWidget, Label *label,
                                   QWidget *box)
{
    if (!layout || !subWidget || !label || !box)
        return;

    const int spacing = qMax(0, subWidget->width() / 2 - label->width());
    layout->setSpacing(spacing);
    // 数值框宽度取标签与间距占完后的剩余空间（标签未变宽时仍为行宽一半）
    box->setFixedWidth(qMax(1, subWidget->width() - label->width() - spacing));
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

    fitLabelSize(baseUrlLabel);
    fitLabelSize(apiKeyLabel);
    fitLabelSize(modelNameLabel);

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

    fitLabelSize(maxTokensLabel);
    fitLabelSize(topPLabel);
    fitLabelSize(temperatureLabel);
    layoutValueRow(maxTokensTopSubHLayout, maxTokensTopSubWidget, maxTokensLabel, maxTokensBox);
    layoutValueRow(topPTopSubHLayout, topPTopSubWidget, topPLabel, topPBox);
    layoutValueRow(temperatureTopSubHLayout, temperatureTopSubWidget, temperatureLabel,
                   temperatureBox);
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

    // 布局快照：确认各部件尺寸与布局距离是否按 DPI 比例缩放（跨 DPI 时重点看这段）
    qDebug().nospace().noquote() << "SettingWidget::setSize 面板=" << sizeInfo(this)
                                 << " settingVLayout " << layoutInfo(settingVLayout);
    qDebug().nospace().noquote() << "  modelSelectWidget=" << sizeInfo(modelSelectWidget)
                                 << " modelSelectVLayout " << layoutInfo(modelSelectVLayout);
    qDebug().nospace().noquote() << "    baseUrlWidget=" << sizeInfo(baseUrlWidget)
                                 << " baseUrlHLayout " << layoutInfo(baseUrlHLayout)
                                 << " label=" << sizeInfo(baseUrlLabel)
                                 << " edit=" << sizeInfo(baseUrlEdit);
    qDebug().nospace().noquote() << "    apiKeyWidget=" << sizeInfo(apiKeyWidget)
                                 << " apiKeyHLayout " << layoutInfo(apiKeyHLayout)
                                 << " label=" << sizeInfo(apiKeyLabel)
                                 << " edit=" << sizeInfo(apiKeyEdit);
    qDebug().nospace().noquote() << "    modelNameWidget=" << sizeInfo(modelNameWidget)
                                 << " modelNameHLayout " << layoutInfo(modelNameHLayout)
                                 << " label=" << sizeInfo(modelNameLabel)
                                 << " edit=" << sizeInfo(modelNameEdit);
    qDebug().nospace().noquote() << "  maxTokensWidget=" << sizeInfo(maxTokensWidget)
                                 << " maxTokensVLayout " << layoutInfo(maxTokensVLayout);
    qDebug().nospace().noquote() << "    上子行=" << sizeInfo(maxTokensTopSubWidget)
                                 << " " << layoutInfo(maxTokensTopSubHLayout)
                                 << " label=" << sizeInfo(maxTokensLabel)
                                 << " box=" << sizeInfo(maxTokensBox);
    qDebug().nospace().noquote() << "    下子行=" << sizeInfo(maxTokensBottomSubWidget)
                                 << " " << layoutInfo(maxTokensBottomSubHLayout)
                                 << " slider=" << sizeInfo(maxTokensSlider);
    qDebug().nospace().noquote() << "  topPWidget=" << sizeInfo(topPWidget) << " topPVLayout "
                                 << layoutInfo(topPVLayout);
    qDebug().nospace().noquote() << "    上子行=" << sizeInfo(topPTopSubWidget) << " "
                                 << layoutInfo(topPTopSubHLayout)
                                 << " label=" << sizeInfo(topPLabel)
                                 << " box=" << sizeInfo(topPBox);
    qDebug().nospace().noquote() << "    下子行=" << sizeInfo(topPBottomSubWidget) << " "
                                 << layoutInfo(topPBottomSubHLayout)
                                 << " slider=" << sizeInfo(topPSlider);
    qDebug().nospace().noquote() << "  temperatureWidget=" << sizeInfo(temperatureWidget)
                                 << " temperatureVLayout " << layoutInfo(temperatureVLayout);
    qDebug().nospace().noquote() << "    上子行=" << sizeInfo(temperatureTopSubWidget) << " "
                                 << layoutInfo(temperatureTopSubHLayout)
                                 << " label=" << sizeInfo(temperatureLabel)
                                 << " box=" << sizeInfo(temperatureBox);
    qDebug().nospace().noquote() << "    下子行=" << sizeInfo(temperatureBottomSubWidget) << " "
                                 << layoutInfo(temperatureBottomSubHLayout)
                                 << " slider=" << sizeInfo(temperatureSlider);
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

    // 标签宽高不再按 DPI 比例推导：像素字号经字形 hinting 后文本宽度与
    // 字号并非严格线性，比例推导出的固定宽度在多数缩放比例下小于实际文本
    // 宽度，文本右边缘被部件边缘裁掉（如 "Model" 只显示 "Mode"）。
    // 这里只重设字体，随后按新字体的实际度量重新拟合尺寸
    baseUrlLabel->updateFontSize();
    apiKeyLabel->updateFontSize();
    modelNameLabel->updateFontSize();
    maxTokensLabel->updateFontSize();
    topPLabel->updateFontSize();
    temperatureLabel->updateFontSize();
    fitLabelSize(baseUrlLabel);
    fitLabelSize(apiKeyLabel);
    fitLabelSize(modelNameLabel);
    fitLabelSize(maxTokensLabel);
    fitLabelSize(topPLabel);
    fitLabelSize(temperatureLabel);
    qDebug() << "baseUrlLabel" << baseUrlLabel->size();
    qDebug() << "apiKeyLabel" << apiKeyLabel->size();
    qDebug() << "modelNameLabel" << modelNameLabel->size();
    qDebug() << "maxTokensLabel" << maxTokensLabel->size();
    qDebug() << "topPLabel" << topPLabel->size();
    qDebug() << "temperatureLabel" << temperatureLabel->size();

    // 输入框只缩放高度：宽度由行内布局按剩余空间分配（见 SettingEdit::setHeight）
    baseUrlEdit->setHeight(
            qRound(widgetSizeDict["baseUrlEdit"].value<QSize>().height() * ratio));
    apiKeyEdit->setHeight(
            qRound(widgetSizeDict["apiKeyEdit"].value<QSize>().height() * ratio));
    modelNameEdit->setHeight(
            qRound(widgetSizeDict["modelNameEdit"].value<QSize>().height() * ratio));
    // 输入框样式表内含像素字号 font-size，按新像素字号重应用
    baseUrlEdit->updateFontSize();
    apiKeyEdit->updateFontSize();
    modelNameEdit->updateFontSize();

    baseUrlHLayout->setContentsMargins(
            scaleMargins(widgetSizeDict["baseUrlHLayout contentsMargins"].value<QMargins>()));
    apiKeyHLayout->setContentsMargins(
            scaleMargins(widgetSizeDict["apiKeyHLayout contentsMargins"].value<QMargins>()));
    modelNameHLayout->setContentsMargins(
            scaleMargins(widgetSizeDict["modelNameHLayout contentsMargins"].value<QMargins>()));
    baseUrlHLayout->setSpacing(
            qMax(1, qRound(widgetSizeDict["baseUrlHLayout spacing"].value<int>() * ratio)));
    apiKeyHLayout->setSpacing(
            qMax(1, qRound(widgetSizeDict["apiKeyHLayout spacing"].value<int>() * ratio)));
    modelNameHLayout->setSpacing(
            qMax(1, qRound(widgetSizeDict["modelNameHLayout spacing"].value<int>() * ratio)));

    // 宽度不再按“行宽 - 标签宽 - 间距”反推后修正间距：那个修正会把算出
    // 的（可能是负的）间距写回 widgetSizeDict，下一次缩放继续沿用，导致
    // 标签与输入框重叠或输入框越出行右边缘。行宽与输入框宽度现在由布局
    // 自行分配，这里只按比例缩放行自身的尺寸
    baseUrlWidget->setFixedSize(scale(widgetSizeDict["baseUrlWidget"].value<QSize>()));
    apiKeyWidget->setFixedSize(scale(widgetSizeDict["apiKeyWidget"].value<QSize>()));
    modelNameWidget->setFixedSize(scale(widgetSizeDict["modelNameWidget"].value<QSize>()));
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
    layoutValueRow(maxTokensTopSubHLayout, maxTokensTopSubWidget, maxTokensLabel, maxTokensBox);
    layoutValueRow(topPTopSubHLayout, topPTopSubWidget, topPLabel, topPBox);
    layoutValueRow(temperatureTopSubHLayout, temperatureTopSubWidget, temperatureLabel,
                   temperatureBox);
    maxTokensBox->setSize(maxTokensBox->width(),
                          qRound(widgetSizeDict["maxTokensBox"].value<QSize>().height() * ratio));
    topPBox->setSize(topPBox->width(),
                     qRound(widgetSizeDict["topPBox"].value<QSize>().height() * ratio));
    temperatureBox->setSize(
            temperatureBox->width(),
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
