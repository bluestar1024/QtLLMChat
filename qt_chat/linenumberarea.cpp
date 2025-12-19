#include "linenumberarea.h"
#include "codeeditor.h"

LineNumberArea::LineNumberArea(QWidget *parent)
    : QWidget(parent), editor(qobject_cast<CodeEditor *>(parent))
{
    setDarkTheme();
}

LineNumberArea::~LineNumberArea() { }

void LineNumberArea::setLightTheme()
{
    backgroundColor = QColor(255, 255, 255);
    numberColor = QColor(78, 86, 92);
    update();
}

void LineNumberArea::setDarkTheme()
{
    backgroundColor = QColor(20, 20, 28);
    numberColor = QColor(178, 170, 164);
    update();
}

void LineNumberArea::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    QPainterPath path;
    path.setFillRule(Qt::WindingFill);
    QRect rect = this->rect();
    path.addRoundedRect(rect, 7, 7);
    path.addRect(rect.x(), rect.y(), 7, 7);
    path.addRect(rect.width() - 7, rect.y(), 7, 7);
    path.addRect(rect.width() - 7, rect.height() - 7, 7, 7);

    painter.setPen(Qt::NoPen);
    painter.setBrush(QBrush(backgroundColor));
    painter.drawPath(path.simplified());

    painter.setPen(QPen(numberColor));
    painter.setFont(editor->font());

    int hAdvance = editor->fontMetrics().horizontalAdvance('9');
    int blockNumber = editor->getFirstVisibleBlockNumber();
    QTextBlock block = editor->document()->findBlockByNumber(blockNumber);
    int top = static_cast<int>(editor->document()
                                       ->documentLayout()
                                       ->blockBoundingRect(block)
                                       .translated(0, -editor->verticalScrollBar()->value())
                                       .top());
    int bottom = top
            + static_cast<int>(
                         editor->document()->documentLayout()->blockBoundingRect(block).height());

    while (block.isValid() && top <= event->rect().bottom()) {
        if (block.isVisible() && bottom >= event->rect().top()) {
            QString number = QString::number(blockNumber + 1);
            int x = static_cast<int>(
                    (2 - QString::number(editor->document()->blockCount()).length()) * 0.275
                    * hAdvance);
            int lineHeight = static_cast<int>(block.layout()->lineAt(0).rect().height());
            painter.drawText(x, top, width() - hAdvance, lineHeight,
                             Qt::AlignRight | Qt::AlignVCenter, number);
        }

        block = block.next();
        top = static_cast<int>(editor->document()
                                       ->documentLayout()
                                       ->blockBoundingRect(block)
                                       .translated(0, -editor->verticalScrollBar()->value())
                                       .top());
        bottom = top
                + static_cast<int>(
                         editor->document()->documentLayout()->blockBoundingRect(block).height());
        ++blockNumber;
    }
}
