#ifndef CHATRECORDSWIDGET_H
#define CHATRECORDSWIDGET_H

#pragma once
#include "pushbutton.h"
#include "widget.h"
#include "lineedit.h"

#include <QtWidgets/QLabel>
#include <QtWidgets/QListWidget>
#include <QtWidgets/QListWidgetItem>
#include <QtWidgets/QBoxLayout>
#include <QtGui/QPainter>
#include <QtGui/QPainterPath>
#include <QtCore/QSize>
#include <QtWidgets/QSizePolicy>
#include <QtGui/QIcon>
#include <QtGui/QBrush>
#include <QtGui/QColor>
#include <QtCore/QVariant>
#include <QtCore/QMap>

class AppContext;

class ChatRecordsWidget : public QWidget
{
    Q_OBJECT
public:
    explicit ChatRecordsWidget(AppContext *appContext, QWidget *parent = nullptr);
    ~ChatRecordsWidget();

    template <typename T>
    void connectSettingButtonClick(T *receiver, void (T::*slot)());
    template <typename T>
    void connectSettingButtonClick(T *receiver, void (T::*slot)(bool));
    template <typename T>
    void connectLineEditTextChanged(T *receiver, void (T::*slot)());
    template <typename T>
    void connectLineEditTextChanged(T *receiver, void (T::*slot)(const QString &));
    template <typename T>
    void connectSearchButtonClick(T *receiver, void (T::*slot)());
    template <typename T>
    void connectSearchButtonClick(T *receiver, void (T::*slot)(bool));
    template <typename T>
    void connectClearAllButtonClick(T *receiver, void (T::*slot)());
    template <typename T>
    void connectClearAllButtonClick(T *receiver, void (T::*slot)(bool));
    template <typename T>
    void connectListItemClick(T *receiver, void (T::*slot)(QListWidgetItem *));

    void setSize();
    void saveWidgetSize();
    void resetWidgetSize();
    // 按 144 DPI 基准（DpiUtil::kBaseDpi）重设尺寸、边距与列表项高度
    void updateSize(qreal curDpi, qreal lastDpi);
    QString getLineEditText() const;
    QListWidgetItem *addListItem(const QString &string);
    void delAllListItems();
    void listItemSetData(QListWidgetItem *item, const QString &string);
    QString listItemToString(QListWidgetItem *item) const;

protected:
    void paintEvent(QPaintEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;

private:
    AppContext *appContext;
    QFont font;
    QFont titleFont;
    QString fontFamily;
    PushButton *settingButton;
    QString settingImagesPath;
    Widget *buttonWidget;
    QVBoxLayout *buttonVLayout;
    QLabel *label;
    Widget *headWidget;
    QHBoxLayout *headHLayout;
    LineEdit *lineEdit;
    PushButton *clearAllButton;
    QString clearAllImagesPath;
    QString clearAllHoverImagesPath;
    QWidget *searchWidget;
    QHBoxLayout *searchHLayout;
    QListWidget *listWidget;
    Widget *mainWidget;
    QVBoxLayout *mainVLayout;
    QMap<QString, QVariant> widgetSizeDict;
    // widgetSizeDict 以 144 DPI 基准保存：保存时按当前已应用的 DPI 折算回基准，
    // 读取时再按目标 DPI 放大，来回跨屏不会累积取整误差
    qreal appliedDpi;
    QListWidgetItem *chatRecordItem;
};

template <typename T>
void ChatRecordsWidget::connectSettingButtonClick(T *receiver, void (T::*slot)())
{
    connect(settingButton, &QPushButton::clicked, [=](bool) { (receiver->*slot)(); });
}

template <typename T>
void ChatRecordsWidget::connectSettingButtonClick(T *receiver, void (T::*slot)(bool))
{
    connect(settingButton, &QPushButton::clicked, receiver, slot);
}

template <typename T>
void ChatRecordsWidget::connectLineEditTextChanged(T *receiver, void (T::*slot)())
{
    connect(lineEdit, &QLineEdit::textChanged, [=](const QString &) { (receiver->*slot)(); });
}

template <typename T>
void ChatRecordsWidget::connectLineEditTextChanged(T *receiver, void (T::*slot)(const QString &))
{
    connect(lineEdit, &QLineEdit::textChanged, receiver, slot);
}

template <typename T>
void ChatRecordsWidget::connectSearchButtonClick(T *receiver, void (T::*slot)())
{
    lineEdit->connectSearchButtonClick(receiver, slot);
}

template <typename T>
void ChatRecordsWidget::connectSearchButtonClick(T *receiver, void (T::*slot)(bool))
{
    lineEdit->connectSearchButtonClick(receiver, slot);
}

template <typename T>
void ChatRecordsWidget::connectClearAllButtonClick(T *receiver, void (T::*slot)())
{
    connect(clearAllButton, &QPushButton::clicked, [=](bool) { (receiver->*slot)(); });
}

template <typename T>
void ChatRecordsWidget::connectClearAllButtonClick(T *receiver, void (T::*slot)(bool))
{
    connect(clearAllButton, &QPushButton::clicked, receiver, slot);
}

template <typename T>
void ChatRecordsWidget::connectListItemClick(T *receiver, void (T::*slot)(QListWidgetItem *))
{
    connect(listWidget, &QListWidget::itemClicked, receiver, slot);
}

#endif // CHATRECORDSWIDGET_H
