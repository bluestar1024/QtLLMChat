#include "spinbox.h"
#include "appcontext.h"
#include "dpiutil.h"

#include <QtCore/QDebug>
#include <QtGui/QScreen>

namespace {
// 以 144 DPI 屏上的实测物理尺寸为基准（DpiUtil::kBaseDpi），其它 DPI 按
// dpi / 144 缩放后取整，让两个屏看到的物理尺寸一致（96 DPI 正好是 2/3：边框
// 2→1、圆角 8→5、两按钮之间的空隙 2→1）
constexpr int kBaseBorderWidth = 2;
constexpr int kBaseRadius = 8;
constexpr int kBaseGap = 2;
// 按钮内箭头四周的留白：image 由 drawRule 画在按钮的 contentsRect 里（
// qstylesheetstyle.cpp:1428），所以 padding 就是箭头四周的空白。padding 同时
// 会被计入按钮矩形尺寸（QRenderRule::size() = 内容 + padding），所以下面按
// “按钮外形 - 留白”反算样式表里的 width/height。
// 上按钮的上侧、下按钮的下侧朝控件外缘，留白给大一点；朝两个按钮中间的那侧
// 给小一点，两个箭头视觉上会向控件中线靠拢
constexpr int kBaseOuterImagePadding = 3;
constexpr int kBaseInnerImagePadding = 2;
// 144 DPI 基准下按钮高 13 = 内容 8 + 外留白 3 + 内留白 2。留白合计(5)与按钮高
// (13)的比例是下面按比例分配留白的依据：留白必须随按钮高度等比，不能各自按
// “基准像素 × scale”取整后再相减——125%（120 DPI）时 3→3、2→2 合计 5px 会
// 占掉半个按钮高，内容区只剩 5px，箭头反而比 100% 时的 6px 还小
constexpr int kBaseButtonHeight = 13;
constexpr int kBasePaddingSum = kBaseOuterImagePadding + kBaseInnerImagePadding;
// 箭头图 375:256 的长宽比：内容区宽度由高度按它反推，比例对不上时 image
// 会被拉伸，箭头会变形
constexpr qreal kArrowAspect = 375.0 / 256.0;

// 当前控件所在屏相对基准 DPI 的缩放比例；取不到屏时按 1（基准）处理
qreal dpiScale(const QWidget *widget)
{
    const QScreen *screen = widget->screen();
    const qreal dpi = screen ? screen->logicalDotsPerInch() : 0;
    return DpiUtil::scaleOf(dpi);
}

// 样式表 url() 只认正斜杠（Windows 路径需要转换）
QString imageUrl(const QString &path)
{
    return QString(path).replace(QLatin1Char('\\'), QLatin1Char('/'));
}
} // namespace

SpinBox::SpinBox(AppContext *appContext, QWidget *parent)
    : QSpinBox(parent),
      appContext(appContext),
      styledWidth(-1),
      styledHeight(-1),
      styledFontPixelSize(-1),
      styledDpiScale(-1)
{
    resize(170, 32);
    setCursor(Qt::PointingHandCursor);
    setAlignment(Qt::AlignHCenter);

    applyStyleSheet();
}

SpinBox::~SpinBox() { }

void SpinBox::applyStyleSheet()
{
    const int fontPixelSize = appContext->windowFontPixelSize();
    const qreal scale = dpiScale(this);
    const int scalePermille = qRound(scale * 1000);
    // 宽、高、像素字号与 DPI 比例都未变时跳过：resizeEvent 里改样式表会再次
    // 触发布局，避免无谓的循环；字号参与比较是跨 DPI 后的兜底（高度取整后
    // 可能不变），DPI 比例参与比较则覆盖同字号不同屏的情况
    if (styledHeight == height() && styledWidth == width() && styledFontPixelSize == fontPixelSize
        && styledDpiScale == scalePermille)
        return;
    styledHeight = height();
    styledWidth = width();
    styledFontPixelSize = fontPixelSize;
    styledDpiScale = scalePermille;

    const int borderWidth = qMax(1, qRound(kBaseBorderWidth * scale));
    const int radius = qMax(2, qRound(kBaseRadius * scale));
    const int gap = qMax(1, qRound(kBaseGap * scale));
    // 按钮高度受控件实际高度约束：扣掉上下边框与中间空隙后对半分，
    // 控件被布局压扁或拉高时按钮跟着变，既不重叠也不溢出
    const int buttonHeight = qMax(2, (height() - 2 * borderWidth - gap) / 2);
    // 留白合计固定占按钮高的同一比例（144 基准 5/13），内容区稳定占 8/13，
    // 箭头随按钮等比缩放，不会出现“缩放比从 100% 提到 125% 后留白吃掉更多
    // 高度、箭头反而变小”；外/内按 3:2 分，保留外大内小
    const int paddingSum = qMax(0, buttonHeight * kBasePaddingSum / kBaseButtonHeight);
    const int outerImagePadding =
            qMin(paddingSum, qRound(paddingSum * qreal(kBaseOuterImagePadding) / kBasePaddingSum));
    const int innerImagePadding = paddingSum - outerImagePadding;
    const int imageHeight = buttonHeight - paddingSum;
    // 内容区宽度由高度按箭头长宽比反推；控件被压得很窄时再压回半个控件宽，
    // 避免按钮宽过一半盖住数字（限幅后 image 会等比缩小，不会变形）
    const int maxImageWidth = qMax(3, (width() - 2 * borderWidth) / 2 - 2 * outerImagePadding);
    const int imageWidth = qMax(3, qMin(qRound(imageHeight * kArrowAspect), maxImageWidth));

    // 上下按钮必须同时钉住位置并预留间隙：样式表只给 width/height 时，
    // QCommonStyle 仍按“上按钮占上半、下按钮占下半”摆放，按钮高度取控件
    // 半高时正好填满整个控件高度，两个按钮在控件中部重叠，点击下按钮的
    // 上沿实际命中上按钮（表现为只能点到上按钮）。这里用 subcontrol-position
    // 把上按钮钉在顶部、下按钮钉在底部，并在中间留出 gap
    //
    // 注意 padding 不能用来平移按钮：它不参与 originRect，只会计入按钮矩形尺寸
    // （上面已把 width/height 扣掉留白，所以这里的 padding 不会改动按钮外形，
    // 只把 image 的画布缩到 contentsRect 内形成留白）
    //
    // 留白只能用 Qt 认得的写法：padding 简写（上 右 下 左，qcssparser.cpp:540
    // case Padding）或 padding-top/-right/-bottom/-left。写成 top-padding 这种
    // 属性名会被当成未知属性整条丢弃，padding 失效而 width/height 已经扣过，
    // 按钮就会缩小、位置跟着变
    // 上按钮：上侧朝控件外缘用大留白；下按钮反过来，两者都以中线一侧为小留白
    setStyleSheet(QString(R"(
        QSpinBox {
            border: %1px solid;
            border-radius: %2px;
            background: transparent;
            font: %3px;
        }
        QSpinBox::up-button {
            subcontrol-origin: padding;
            subcontrol-position: top right;
            width: %4px;
            height: %5px;
            padding: %6px %6px %7px %6px;
            background: transparent;
            image: url(%8);
        }
        QSpinBox::up-button:pressed {
            margin-top: 1px;
        }
        QSpinBox::down-button {
            subcontrol-origin: padding;
            subcontrol-position: bottom right;
            width: %4px;
            height: %5px;
            padding: %7px %6px %6px %6px;
            background: transparent;
            image: url(%9);
        }
        QSpinBox::down-button:pressed {
            margin-bottom: 1px;
        }
        /* 箭头改由按钮的 image 提供，这里去掉样式默认绘制的原生箭头 */
        QSpinBox::up-arrow,
        QSpinBox::down-arrow {
            image: none;
            width: 0px;
            height: 0px;
        }
    )")
                          .arg(borderWidth)
                          .arg(radius)
                          .arg(fontPixelSize)
                          .arg(imageWidth)
                          .arg(imageHeight)
                          .arg(outerImagePadding)
                          .arg(innerImagePadding)
                          .arg(imageUrl(appContext->imagesDir() + "/up_arrow.png"))
                          .arg(imageUrl(appContext->imagesDir() + "/down_arrow.png")));

    // 只在尺寸真正变化时输出：跨 DPI 时用来核对两档物理尺寸是否成比例
    qDebug().nospace().noquote() << "SpinBox 样式 scale=" << scale << " 控件=" << width() << "x"
                                 << height() << " border=" << borderWidth << " radius=" << radius
                                 << " gap=" << gap
                                 << " 按钮=" << imageWidth + 2 * outerImagePadding << "x"
                                 << buttonHeight << " 留白(外/内)=" << outerImagePadding << "/"
                                 << innerImagePadding << " 内容=" << imageWidth << "x"
                                 << imageHeight;
}

void SpinBox::setSize(int width, int height)
{
    setFixedSize(width, height);
    applyStyleSheet();
}

void SpinBox::mousePressEvent(QMouseEvent *event)
{
    QSpinBox::mousePressEvent(event);
    event->ignore();
}

void SpinBox::mouseReleaseEvent(QMouseEvent *event)
{
    QSpinBox::mouseReleaseEvent(event);
    event->ignore();
}

void SpinBox::resizeEvent(QResizeEvent *event)
{
    QSpinBox::resizeEvent(event);
    // 控件高度由行布局决定（未调用 setSize 时也会变化），按钮尺寸需随之重算
    applyStyleSheet();
}
