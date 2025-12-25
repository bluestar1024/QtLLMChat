#include "codeshow.h"

#include <QtGui/QIcon>
#include <QtGui/QPalette>
#include <QtGui/QTextOption>

CodeShow::CodeShow(const QString &codeText, const QString &lexerName, int maxWidth, QWidget *parent)
    : QWidget(parent),
      codeText(codeText),
      lexerName(lexerName),
      maxWidth(maxWidth),
      isLightThemeStyle(false),
      isWordWrap(true)
{
    resize(this->maxWidth + 2, 40);
    setupUI();
}

CodeShow::~CodeShow() { }

void CodeShow::setupUI()
{
    topWidget = new QWidget();
    topSubLeftWidget = new QWidget();
    topSubRightWidget = new QWidget();

    label = new QLabel(lexerName);
    int fontId = QFontDatabase::addApplicationFont(fontFilePath);
    if (fontId != -1) {
        QString fontFamily = QFontDatabase::applicationFontFamilies(fontId).at(0);
        QFont font(fontFamily, windowFontPointSize);
        label->setFont(font);
    }
    QPalette pal = label->palette();
    pal.setColor(QPalette::Text, Qt::white);
    label->setPalette(pal);
    label->adjustSize();

    int topHeight = qMax(label->height(), 24);
    topWidget->setFixedHeight(topHeight);
    topSubLeftWidget->setFixedHeight(topHeight);
    topSubRightWidget->setFixedHeight(topHeight);

    toggleThemeButton = new PushButton("日间主题", 15, 35);
    toggleThemeButton->setFixedSize(topHeight - 10, topHeight - 10);
    lightThemeImagesPath = imageDir + "/light_theme.png";
    darkThemeImagesPath = imageDir + "/dark_theme.png";
    toggleThemeButton->setIcon(QIcon(lightThemeImagesPath));
    toggleThemeButton->setIconSize(QSize(topHeight - 10, topHeight - 10));
    toggleThemeButton->setStyleSheet("border: none; background: transparent;");
    connect(toggleThemeButton, &QPushButton::clicked, this, &CodeShow::toggleThemeStyle);

    wordWrapButton = new PushButton("折叠成单行", 15, 35);
    wordWrapButton->setFixedSize(topHeight - 10, topHeight - 10);
    lightWordWrapImagesPath = imageDir + "/light_word_wrap.png";
    lightSingleLineImagesPath = imageDir + "/light_single_line.png";
    darkWordWrapImagesPath = imageDir + "/dark_word_wrap.png";
    darkSingleLineImagesPath = imageDir + "/dark_single_line.png";
    wordWrapButton->setIcon(QIcon(lightSingleLineImagesPath));
    wordWrapButton->setIconSize(QSize(topHeight - 10, topHeight - 10));
    wordWrapButton->setStyleSheet("border: none; background: transparent;");
    connect(wordWrapButton, &QPushButton::clicked, this, &CodeShow::setLineWordWrapMode);

    codeCopyButton = new PushButton("复制代码", 15, 35);
    codeCopyButton->setFixedSize(topHeight - 10, topHeight - 10);
    lightCodeCopyImagesPath = imageDir + "light_code_copy.png";
    darkCodeCopyImagesPath = imageDir + "dark_code_copy.png";
    codeCopyButton->setIcon(QIcon(lightCodeCopyImagesPath));
    codeCopyButton->setIconSize(QSize(topHeight - 10, topHeight - 10));
    codeCopyButton->setStyleSheet("border: none; background: transparent;");
    connect(codeCopyButton, &QPushButton::clicked, this, &CodeShow::copyCode);

    QHBoxLayout *topSubLeftHLayout = new QHBoxLayout(topSubLeftWidget);
    topSubLeftHLayout->setAlignment(Qt::AlignLeft);
    topSubLeftHLayout->setContentsMargins(0, (topHeight - label->height()) / 2, 0,
                                          (topHeight - label->height()) / 2);
    topSubLeftHLayout->addWidget(label);

    QHBoxLayout *topSubRightHLayout = new QHBoxLayout(topSubRightWidget);
    topSubRightHLayout->setAlignment(Qt::AlignRight);
    topSubRightHLayout->setContentsMargins(0, 5, 0, 5);
    topSubRightHLayout->setSpacing(5);
    topSubRightHLayout->addWidget(toggleThemeButton);
    topSubRightHLayout->addWidget(wordWrapButton);
    topSubRightHLayout->addWidget(codeCopyButton);

    QHBoxLayout *topHLayout = new QHBoxLayout(topWidget);
    topHLayout->addWidget(topSubLeftWidget, 0, Qt::AlignLeft);
    topHLayout->addWidget(topSubRightWidget, 0, Qt::AlignRight);
    topHLayout->setContentsMargins(10, 0, 10, 0);

    codeEdit = new CodeEditor(maxWidth);
    codeEdit->highlightCode(codeText, lexerName);
    connect(codeEdit, &CodeEditor::setSizeFinished,
            [this]() { setFixedSize(maxWidth + 2, codeEdit->height() + topWidget->height() + 2); });

    QVBoxLayout *mainVLayout = new QVBoxLayout(this);
    mainVLayout->addWidget(topWidget);
    mainVLayout->addWidget(codeEdit);
    mainVLayout->setContentsMargins(1, 1, 1, 1);
    mainVLayout->setSpacing(0);

    applyDarkStyle();
}

void CodeShow::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    QPen pen(borderColor);
    painter.setPen(pen);
    painter.drawRoundedRect(rect(), 10, 10);
}

QString CodeShow::getText() const
{
    return codeText;
}
QString CodeShow::getLexerName() const
{
    return lexerName;
}

void CodeShow::setText(const QString &codeText, const QString &lexerName)
{
    this->codeText = codeText;
    this->lexerName = lexerName;
    codeEdit->highlightCode(codeText, lexerName);
    codeEdit->setFixedWidth(maxWidth);
}

bool CodeShow::hasSelectedText() const
{
    return codeEdit->textCursor().hasSelection();
}

QString CodeShow::getSelectedText() const
{
    return codeEdit->textCursor().selectedText();
}

void CodeShow::toggleThemeStyle()
{
    isLightThemeStyle = !isLightThemeStyle;
    if (isLightThemeStyle)
        applyLightStyle();
    else
        applyDarkStyle();
    codeEdit->setThemeStyle(isLightThemeStyle);
    repaint();
}

void CodeShow::applyDarkStyle()
{
    topWidget->setStyleSheet("background-color: #34343c; border-top-left-radius: 7px; "
                             "border-top-right-radius: 7px;");
    QPalette pal = label->palette();
    pal.setColor(QPalette::Text, QColor(Qt::white));
    label->setPalette(pal);
    toggleThemeButton->setIcon(QIcon(lightThemeImagesPath));
    toggleThemeButton->setTipText("日间主题");
    wordWrapButton->setIcon(
            QIcon(isWordWrap ? lightSingleLineImagesPath : lightWordWrapImagesPath));
    codeCopyButton->setIcon(QIcon(lightCodeCopyImagesPath));
    borderColor = QColor(Qt::transparent);
}

void CodeShow::applyLightStyle()
{
    topWidget->setStyleSheet("background-color: #f3f4f3; border-top-left-radius: 7px; "
                             "border-top-right-radius: 7px;");
    QPalette pal = label->palette();
    pal.setColor(QPalette::Text, QColor(Qt::black));
    label->setPalette(pal);
    toggleThemeButton->setIcon(QIcon(darkThemeImagesPath));
    toggleThemeButton->setTipText("夜间主题");
    wordWrapButton->setIcon(QIcon(isWordWrap ? darkSingleLineImagesPath : darkWordWrapImagesPath));
    codeCopyButton->setIcon(QIcon(darkCodeCopyImagesPath));
    borderColor = QColor(196, 196, 212);
}

void CodeShow::setLineWordWrapMode()
{
    isWordWrap = !isWordWrap;
    if (isWordWrap) {
        codeEdit->setWordWrapMode(QTextOption::WordWrap);
        codeEdit->setLineWrapMode(QTextEdit::WidgetWidth);
        wordWrapButton->setIcon(
                QIcon(isLightThemeStyle ? darkSingleLineImagesPath : lightSingleLineImagesPath));
        wordWrapButton->setTipText("折叠成单行");
    } else {
        codeEdit->setWordWrapMode(QTextOption::NoWrap);
        codeEdit->setLineWrapMode(QTextEdit::NoWrap);
        wordWrapButton->setIcon(
                QIcon(isLightThemeStyle ? darkWordWrapImagesPath : lightWordWrapImagesPath));
        wordWrapButton->setTipText("自动换行");
    }
    codeEdit->adjustSize();
}

void CodeShow::copyCode()
{
    QApplication::clipboard()->setText(codeText);
}
