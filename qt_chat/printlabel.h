#ifndef PRINTLABEL_H
#define PRINTLABEL_H

#pragma once
#include <QtCore/QTimer>
#include <QtGui/QBrush>
#include <QtGui/QColor>
#include <QtGui/QFont>
#include <QtGui/QFontDatabase>
#include <QtGui/QFontMetricsF>
#include <QtGui/QPainter>
#include <QtGui/QPainterPath>
#include <QtGui/QPalette>
#include <QtGui/QPen>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QWidget>

class PrintLabel : public QWidget
{
    Q_OBJECT
public:
    explicit PrintLabel(const QString &text, QWidget *parent = nullptr);
    ~PrintLabel();

    void updateSize(int curDpi, int lastDpi);
    void setText(const QString &text);
    void printStart();
    void printEnd();

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    QString text;
    QLabel *label;
    QFont font;
    QFontMetrics *fontMetrics;
    QPalette palette;
    QHBoxLayout *mainHLayout;
    QTimer *printTimer;
};

#endif // PRINTLABEL_H
