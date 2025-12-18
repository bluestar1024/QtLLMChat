#ifndef THINKINGBUTTON_H
#define THINKINGBUTTON_H

#include <QtWidgets/QWidget>
#include <QtWidgets/QLabel>
#include <QtWidgets/QHBoxLayout>
#include <QtCore/QTime>
#include <QtGui/QColor>
#include <QtGui/QFont>
#include <QtGui/QFontDatabase>
#include <QtGui/QMouseEvent>
#include <QtGui/QPainter>
#include <QtGui/QPainterPath>
#include <QtGui/QBrush>

extern const int windowFontPixelSize;
extern const QString fontFilePath;
extern const QString imagesDir;

class ThinkingButton : public QWidget
{
    Q_OBJECT
public:
    explicit ThinkingButton(QWidget *parent = nullptr);
    ~ThinkingButton();

    template <typename T>
    void connectButtonClick(T *receiver, void (T::*slot)(bool));
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

    bool isShowThinkContent;
    QColor backgroundColor;
    QTime startThinkTime;

    int thinkTimeLength;

    QLabel *textLabel;
    QLabel *leftIconLabel;
    QLabel *rightIconLabel;

    QString thinkingIconPath;
    QString arrowUpPath;
    QString arrowDownPath;
};

template <typename T>
void ThinkingButton::connectButtonClick(T *receiver, void (T::*slot)(bool))
{
    connect(this, &ThinkingButton::clicked, receiver, slot);
}

#endif // THINKINGBUTTON_H
