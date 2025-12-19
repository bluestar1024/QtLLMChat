#ifndef LINENUMBERAREA_H
#define LINENUMBERAREA_H

#include <QtWidgets/QWidget>
#include <QtGui/QColor>
#include <QtGui/QPainter>
#include <QtGui/QPainterPath>
#include <QtGui/QTextBlock>
#include <QtGui/QFontMetrics>

class CodeEditor;

class LineNumberArea : public QWidget
{
    Q_OBJECT
public:
    explicit LineNumberArea(QWidget *parent = nullptr);
    ~LineNumberArea();

    void setLightTheme();
    void setDarkTheme();

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    CodeEditor *editor;
    QColor backgroundColor;
    QColor numberColor;
};

#endif // LINENUMBERAREA_H
