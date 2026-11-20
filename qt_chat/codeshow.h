#ifndef CODESHOW_H
#define CODESHOW_H

#pragma once
#include "codeeditor.h"
#include "pushbutton.h"

#include <QtWidgets/QWidget>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QTextEdit>
#include <QtGui/QClipboard>
#include <QtGui/QPainter>
#include <QtWidgets/QApplication>

// extern const QString imagesDir;
// extern const QString fontFilePath;
// extern const int windowFontPointSize;

class AppContext;

class CodeShow : public QWidget
{
    Q_OBJECT

public:
    explicit CodeShow(AppContext *appContext, const QString &codeText,
                      const QString &lexerName = "cpp",
                      std::function<void()> sizeFinishFun = nullptr, int maxWidth = 810,
                      QWidget *parent = nullptr);
    ~CodeShow();

    void connectCodeCopyButtonClick(std::function<void()> fun);
    void connectCodeCopyButtonClick(std::function<void(bool)> fun);
    QString getText() const;
    void setText(const QString &codeText, const QString &lexerName = "cpp");
    QString getLexerName() const;
    bool hasSelectedText() const;
    QString getSelectedText() const;
    // 跨 DPI 屏拖动未释放鼠标时的轻量刷新（不重建控件）：语言标签/工具栏
    // 按新像素字号重算，代码区字体重设并重新量算高度，宽度按新的最大宽度设置
    void updateSize(qreal curDpi, qreal lastDpi, int newMaxWidth);
    // 当前最大宽度（跨 DPI 刷新时用于反推创建时相对 MessageWidget 的宽度偏移）
    int getMaxWidth() const { return maxWidth; }

public slots:
    void toggleThemeStyle();
    void setLineWordWrapMode();
    void copyCode();

protected:
    void paintEvent(QPaintEvent *event) override;

signals:
    void setSizeFinished();

private:
    void setupUI();
    void applyDarkStyle();
    void applyLightStyle();

    AppContext *appContext;

    QString codeText;
    QString lexerName;
    std::function<void()> sizeFinishFun;
    int maxWidth;

    QWidget *topWidget;
    QWidget *topSubLeftWidget;
    QWidget *topSubRightWidget;
    QLabel *label;
    PushButton *toggleThemeButton;
    PushButton *wordWrapButton;
    PushButton *codeCopyButton;
    CodeEditor *codeEdit;

    bool isLightThemeStyle;
    bool isWordWrap;

    QString lightThemeImagesPath;
    QString darkThemeImagesPath;
    QString lightWordWrapImagesPath;
    QString lightSingleLineImagesPath;
    QString darkWordWrapImagesPath;
    QString darkSingleLineImagesPath;
    QString lightCodeCopyImagesPath;
    QString darkCodeCopyImagesPath;

    QColor borderColor;
};

#endif // CODESHOW_H
