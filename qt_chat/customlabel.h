#ifndef CUSTOMLABEL_H
#define CUSTOMLABEL_H

#include <QtWidgets/QLabel>
#include <QtGui/QMouseEvent>
#include <QtGui/QContextMenuEvent>

class CustomLabel : public QLabel
{
    Q_OBJECT
public:
    explicit CustomLabel(QWidget *parent = nullptr);
    ~CustomLabel();

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void contextMenuEvent(QContextMenuEvent *event) override;
};

#endif // CUSTOMLABEL_H
