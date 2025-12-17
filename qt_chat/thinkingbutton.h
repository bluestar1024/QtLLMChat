#ifndef THINKINGBUTTON_H
#define THINKINGBUTTON_H

#include <QWidget>
#include <QLabel>
#include <QHBoxLayout>
#include <QTime>
#include <QColor>
#include <QFont>
#include <QFontDatabase>
#include <QMouseEvent>
#include <QPainter>
#include <QPainterPath>
#include <QBrush>

extern const int windowFontPixelSize;
extern const QString fontFilePath;
extern const QString imagesDir;

class ThinkingButton : public QWidget
{
    Q_OBJECT
public:
    explicit ThinkingButton(QWidget *parent = nullptr);

    void setIsShowThinkContent(bool show);
    void setThinkEnd();
    void setThinkTimeLength(int secs);
    int getThinkTimeLength();

signals:
    void clicked();

protected:
    void enterEvent(QEnterEvent *) override;
    void leaveEvent(QEvent *) override;
    void paintEvent(QPaintEvent *) override;
    void mousePressEvent(QMouseEvent *e) override;

private:
    void initUI();
    void updateSize();

    bool isShowThinkContent = true;
    QColor backgroundColor = QColor(248, 248, 248);
    QTime startThinkTime;

    int thinkTimeLength = 0;

    QLabel *textLabel = nullptr;
    QLabel *leftIconLabel = nullptr;
    QLabel *rightIconLabel = nullptr;

    QString thinkingIconPath;
    QString arrowUpPath;
    QString arrowDownPath;
};

#endif // THINKINGBUTTON_H
