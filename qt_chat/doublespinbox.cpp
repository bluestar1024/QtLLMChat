#include "doublespinbox.h"
#include "appcontext.h"

#include <QtCore/QDebug>
#include <QtGui/QScreen>

namespace {
// 以 144 DPI 屏上的实测物理尺寸为基准，其它 DPI 按 dpi / 144 缩放后取整，
// 让两个屏看到的物理尺寸一致（96 DPI 正好是 2/3：边框 2→1、圆角 8→5、
// 按钮 18x13→12x9、两按钮之间的空隙 2→1）
constexpr int kBaseDpi = 144;
constexpr int kBaseBorderWidth = 2;
constexpr int kBaseRadius = 8;
constexpr int kBaseButtonWidth = 18;
constexpr int kBaseButtonHeight = 13;
constexpr int kBaseGap = 2;
// 箭头图 375:256 的长宽比：控件被布局压扁时用它限制按钮宽度，
// 避免按钮变瘦高后箭头被挤成细条
constexpr qreal kArrowAspect = 375.0 / 256.0;

// 当前控件所在屏相对基准 DPI 的缩放比例；取不到屏时按 1（基准）处理
qreal dpiScale(const QWidget *widget)
{
    const QScreen *screen = widget->screen();
    const qreal dpi = screen ? screen->logicalDotsPerInch() : 0;
    return dpi > 0 ? dpi / qreal(kBaseDpi) : 1.0;
}

// 样式表 url() 只认正斜杠（Windows 路径需要转换）
QString imageUrl(const QString &path)
{
    return QString(path).replace(QLatin1Char('\\'), QLatin1Char('/'));
}
} // namespace

DoubleSpinBox::DoubleSpinBox(AppContext *appContext, QWidget *parent)
    : QDoubleSpinBox(parent),
      appContext(appContext),
      styledButtonHeight(-1),
      styledFontPixelSize(-1),
      styledDpiScale(-1)
{
    resize(170, 32);
    setCursor(Qt::PointingHandCursor);
    setAlignment(Qt::AlignHCenter);

    applyStyleSheet();
}

DoubleSpinBox::~DoubleSpinBox() { }

void DoubleSpinBox::applyStyleSheet()
{
    const int fontPixelSize = appContext->windowFontPixelSize();
    const qreal scale = dpiScale(this);
    const int scalePermille = qRound(scale * 1000);
    // 高度、像素字号与 DPI 比例都未变时跳过：resizeEvent 里改样式表会再次
    // 触发布局，避免无谓的循环；字号参与比较是跨 DPI 后的兜底（高度取整后
    // 可能不变），DPI 比例参与比较则覆盖同字号不同屏的情况
    if (styledButtonHeight == height() && styledFontPixelSize == fontPixelSize
        && styledDpiScale == scalePermille)
        return;
    styledButtonHeight = height();
    styledFontPixelSize = fontPixelSize;
    styledDpiScale = scalePermille;

    const int borderWidth = qMax(1, qRound(kBaseBorderWidth * scale));
    const int radius = qMax(2, qRound(kBaseRadius * scale));
    const int gap = qMax(1, qRound(kBaseGap * scale));
    // 按钮高度受控件实际高度约束：扣掉上下边框与中间空隙后对半分，
    // 控件被布局压扁或拉高时按钮跟着变，既不重叠也不溢出
    const int buttonHeight = qMax(2, (height() - 2 * borderWidth - gap) / 2);
    // 宽度按 144 DPI 基准换算，再按箭头长宽比限制一次
    const int buttonWidth
            = qMax(4, qMin(qRound(kBaseButtonWidth * scale), qRound(buttonHeight * kArrowAspect)));

    // 上下按钮必须同时钉住位置并预留间隙：样式表只给 width/height 时，
    // QCommonStyle 仍按“上按钮占上半、下按钮占下半”摆放，按钮高度取控件
    // 半高时正好填满整个控件高度，两个按钮在控件中部重叠，点击下按钮的
    // 上沿实际命中上按钮（表现为只能点到上按钮）。这里用 subcontrol-position
    // 把上按钮钉在顶部、下按钮钉在底部，并在中间留出 gap
    //
    // 注意不能用 padding 做内缩——padding 会把按钮矩形本身撑大（实测
    // width/height 12x9 + padding 1px 会变成 14x11，下按钮上移压住上按钮），
    // 且 padding 计入 QRenderRule::size()，会改坏命中区域
    setStyleSheet(QString(R"(
        QDoubleSpinBox {
            border: %1px solid;
            border-radius: %2px;
            background: transparent;
            font: %3px;
        }
        QDoubleSpinBox::up-button {
            subcontrol-origin: padding;
            subcontrol-position: top right;
            width: %4px;
            height: %5px;
            background: transparent;
            image: url(%6);
        }
        QDoubleSpinBox::up-button:pressed {
            margin-top: 1px;
        }
        QDoubleSpinBox::down-button {
            subcontrol-origin: padding;
            subcontrol-position: bottom right;
            width: %4px;
            height: %5px;
            background: transparent;
            image: url(%7);
        }
        QDoubleSpinBox::down-button:pressed {
            margin-bottom: 1px;
        }
        /* 箭头改由按钮的 image 提供，这里去掉样式默认绘制的原生箭头 */
        QDoubleSpinBox::up-arrow,
        QDoubleSpinBox::down-arrow {
            image: none;
            width: 0px;
            height: 0px;
        }
    )")
                          .arg(borderWidth)
                          .arg(radius)
                          .arg(fontPixelSize)
                          .arg(buttonWidth)
                          .arg(buttonHeight)
                          .arg(imageUrl(appContext->imagesDir() + "/up_arrow.png"))
                          .arg(imageUrl(appContext->imagesDir() + "/down_arrow.png")));

    // 只在尺寸真正变化时输出：跨 DPI 时用来核对两档物理尺寸是否成比例
    qDebug().nospace().noquote() << "DoubleSpinBox 样式 scale=" << scale << " 控件=" << width()
                                 << "x" << height() << " border=" << borderWidth
                                 << " radius=" << radius << " gap=" << gap
                                 << " 按钮=" << buttonWidth << "x" << buttonHeight;
}

void DoubleSpinBox::setSize(int width, int height)
{
    setFixedSize(width, height);
    applyStyleSheet();
}

void DoubleSpinBox::mousePressEvent(QMouseEvent *event)
{
    QDoubleSpinBox::mousePressEvent(event);
    event->ignore();
}

void DoubleSpinBox::mouseReleaseEvent(QMouseEvent *event)
{
    QDoubleSpinBox::mouseReleaseEvent(event);
    event->ignore();
}

void DoubleSpinBox::resizeEvent(QResizeEvent *event)
{
    QDoubleSpinBox::resizeEvent(event);
    // 控件高度由行布局决定（未调用 setSize 时也会变化），按钮尺寸需随之重算
    applyStyleSheet();
}
