#include "texteditfull.h"
#include "appcontext.h"
#include "dpiutil.h"

#include <QtGui/QScreen>

namespace {
// chatInput 自身尺寸以 144 DPI（150% 缩放）屏为基准（DpiUtil::kBaseDpi）：最小
// 高度、高度 sizeHint、边缘宽度（15px 内边距）与边缘圆角（16px）均按
// dpi / 144 换算，让两档屏上物理大小一致；只有 1px 描边内缩不缩放（保证描边
// 仍为 1 像素）
constexpr int kBaseEdgeMargin = 15;
constexpr int kBaseEdgeRadius = 16;
constexpr int kBaseMinimumHeight = 80;
// chatInput 在 144 DPI 屏（基准）下的默认高度：文本区高度取 TextEdit 的项目
// 约定常量 TextEdit::kBaseTextHeight（190），加上下边缘各 15；96 DPI 下换算为
// 147（文本区 127）
constexpr int kBaseHeight = TextEdit::kBaseTextHeight + 2 * kBaseEdgeMargin;
} // namespace

TextEditFull::TextEditFull(AppContext *appContext, QWidget *parent)
    : QWidget(parent),
      appContext(appContext),
      backgroundColorIsLight(false),
      appliedDpi(-1),
      appliedFontPixelSize(-1),
      logNextResize(false),
      edgeMargin(kBaseEdgeMargin),
      edgeRadius(kBaseEdgeRadius)
{
    // 基准值，实际值由 applyDpiScale 按所在屏 DPI 换算
    setMinimumHeight(kBaseMinimumHeight);

    textEdit = new TextEdit(this->appContext);
    mainHLayout = new QHBoxLayout(this);
    mainHLayout->addWidget(textEdit);
    // 边缘宽度基准 15px，实际值由 applyDpiScale 按屏 DPI 换算（resetWidgetSize
    // 中的 2 * edgeMargin 与此对应）
    mainHLayout->setContentsMargins(edgeMargin, edgeMargin, edgeMargin, edgeMargin);

    resize(textEdit->width() + 2 * edgeMargin, kBaseHeight);
    setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);

    bgColor = appContext->textEditFullBGColor();
    bColor = appContext->textEditFullBTColor();

    animationBackgroundColor = new QPropertyAnimation(this, "backgroundColor");
    animationBackgroundColor->setDuration(400);
    animationBackgroundColor->setEasingCurve(QEasingCurve::OutQuad);

    animationBorderColor = new QPropertyAnimation(this, "borderColor");
    animationBorderColor->setDuration(400);
    animationBorderColor->setEasingCurve(QEasingCurve::OutQuad);

    setMouseTracking(true);

    // 构造时窗可能尚未定位到实际屏（screen() 常为主屏），这里先按当前可取得
    // 的屏 DPI 换算一次，窗口落到其他 DPI 的屏后由 MainWindow 的 DPI 缩放链
    // 与 event() 的换屏兜底再次校正
    applyDpiScale(0);
}

TextEditFull::~TextEditFull() { }

bool TextEditFull::event(QEvent *e)
{
    const bool result = QWidget::event(e);
    // 首次显示/窗口换屏时按控件实际所在屏的 DPI 兜底换算一次：MainWindow 的
    // DPI 缩放链是主路径，这里保证缩放链因拖拽节流或时序未到达时部件尺寸
    // 也能自愈（按 144 DPI 基准绝对换算，重复执行结果不变）
    if (e->type() == QEvent::Show || e->type() == QEvent::ScreenChangeInternal)
        applyDpiScale(0);
    return result;
}

void TextEditFull::mouseMoveEvent(QMouseEvent *e)
{
    QWidget::mouseMoveEvent(e);
    e->ignore();
}

void TextEditFull::resizeEvent(QResizeEvent *e)
{
    QWidget::resizeEvent(e);
    // 只在 DPI 换算后的首次 resize 打印：跨屏时核对高度是否按 144 基准换算
    // （文本区高度 = chatInput 高度 - 2 * edgeMargin），不在每次 resize 刷日志
    if (logNextResize) {
        logNextResize = false;
        qDebug().nospace().noquote() << "chatInput 尺寸=" << size() << " 文本区=" << textEdit->size()
                                     << " 最小高=" << minimumHeight() << " dpi=" << appliedDpi;
    }
    qDebug().nospace().noquote() << "chatInput 尺寸=" << size() << " 文本区=" << textEdit->size()
                                     << " 最小高=" << minimumHeight() << " dpi=" << appliedDpi << "print";
}

int TextEditFull::baseHeightPx()
{
    return kBaseHeight;
}

QSize TextEditFull::sizeHint() const
{
    const QSize hint = QWidget::sizeHint();
    // 高度按 144 DPI 基准换算：splitter 中 chatInputWidget（stretch 0）的高度
    // 取自本控件的 sizeHint，不做换算则低 DPI 屏上仍按 144 DPI 的像素高度显示
    const int dpi = appliedDpi > 0 ? appliedDpi : DpiUtil::kBaseDpi;
    return QSize(hint.width(), qMax(minimumHeight(), DpiUtil::scalePx(kBaseHeight, dpi)));
}

QSize TextEditFull::minimumSizeHint() const
{
    const QSize hint = QWidget::minimumSizeHint();
    // 最小高度已由 applyDpiScale 按 144 基准换算：宽度用基类建议，高度用它，
    // 避免内部 TextEdit 的默认最小建议把高度卡在低 DPI 屏上
    return QSize(hint.width(), minimumHeight());
}

void TextEditFull::paintEvent(QPaintEvent *e)
{
    Q_UNUSED(e);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    QPen pen(bColor);
    painter.setPen(pen);

    QBrush brush(Qt::SolidPattern);
    brush.setColor(bgColor);
    painter.setBrush(brush);

    QPainterPath path;
    path.setFillRule(Qt::WindingFill);
    // 圆角按屏 DPI 换算（edgeRadius）；1px 描边内缩不缩放，保证描边仍为 1 像素
    path.addRoundedRect(rect().adjusted(1, 1, -1, -1), edgeRadius, edgeRadius);
    painter.drawPath(path.simplified());

    painter.end();
}

QColor TextEditFull::getBackgroundColor() const
{
    return bgColor;
}

void TextEditFull::setBackgroundColor(const QColor &color)
{
    bgColor = color;
    update();
}

QColor TextEditFull::getBorderColor() const
{
    return bColor;
}

void TextEditFull::setBorderColor(const QColor &color)
{
    bColor = color;
    update();
}

void TextEditFull::backgroundColorShowLight()
{
    if (!backgroundColorIsLight) {
        backgroundColorIsLight = true;
        animationBackgroundColor->setStartValue(appContext->textEditFullBGColor());
        animationBackgroundColor->setEndValue(appContext->textEditFullBGTColor());
        animationBackgroundColor->start();

        animationBorderColor->setStartValue(appContext->textEditFullBTColor());
        animationBorderColor->setEndValue(appContext->textEditFullBColor());
        animationBorderColor->start();
    }
}

void TextEditFull::backgroundColorShowDark()
{
    if (backgroundColorIsLight) {
        backgroundColorIsLight = false;
        animationBackgroundColor->setStartValue(appContext->textEditFullBGTColor());
        animationBackgroundColor->setEndValue(appContext->textEditFullBGColor());
        animationBackgroundColor->start();

        animationBorderColor->setStartValue(appContext->textEditFullBColor());
        animationBorderColor->setEndValue(appContext->textEditFullBTColor());
        animationBorderColor->start();
    }
}

void TextEditFull::clearFocus()
{
    textEdit->clearFocus();
}

void TextEditFull::updateSize(qreal curDpi, qreal lastDpi)
{
    // 尺寸以 144 DPI 为基准从 curDpi 绝对换算，与上一次 DPI 无关：反复跨屏
    // 不会因逐次按比例取整而漂移（lastDpi 仅为接口一致保留）
    Q_UNUSED(lastDpi);
    applyDpiScale(curDpi);
}

void TextEditFull::applyDpiScale(qreal dpi)
{
    if (dpi <= 0) {
        const QScreen *curScreen = screen();
        dpi = curScreen ? curScreen->logicalDotsPerInch() : qreal(DpiUtil::kBaseDpi);
    }
    if (dpi <= 0)
        dpi = qreal(DpiUtil::kBaseDpi);
    const int dpiKey = qRound(dpi);
    const int fontPixelSize = appContext->windowFontPixelSize();
    // DPI 与字号都没变才跳过：换屏自愈会先用旧字号按新 DPI 换算一次，
    // MainWindow 随后更新字号再调 updateSize，只看 DPI 会把内部字体停在旧字号
    if (dpiKey == appliedDpi && fontPixelSize == appliedFontPixelSize)
        return;
    const bool isFirstApply = appliedDpi <= 0;
    appliedDpi = dpiKey;
    appliedFontPixelSize = fontPixelSize;

    // 最小高度、边缘宽度与边缘圆角均按 144 DPI 基准换算：
    // 高 DPI 屏上不会被压得更大/更圆
    setMinimumHeight(DpiUtil::scalePx(kBaseMinimumHeight, dpiKey));
    edgeMargin = DpiUtil::scalePx(kBaseEdgeMargin, dpiKey);
    edgeRadius = DpiUtil::scalePx(kBaseEdgeRadius, dpiKey);
    mainHLayout->setContentsMargins(edgeMargin, edgeMargin, edgeMargin, edgeMargin);

    // sizeHint / minimumSizeHint 已随 DPI 变化，父布局与 splitter 需重算
    // （chatInputWidget 的高度取自 chatInput 的 sizeHint）；构造期首次换算不
    // 触发，避免 splitter 提前按子项当前尺寸而非 sizeHint 分配初始高度
    if (!isFirstApply)
        updateGeometry();
    // 下一轮布局/resize 后打印一次实际高度，供跨屏核对
    logNextResize = true;

    // 内部部件（发送按钮、滚动条、文本区边距与字体）同步换算
    textEdit->applyDpiScale(dpi);
    qDebug().nospace().noquote() << "chatInput DPI 换算 scale=" << DpiUtil::scaleOf(dpi)
                                << " 边缘=" << edgeMargin << " 圆角=" << edgeRadius
                                << " 最小高=" << DpiUtil::scalePx(kBaseMinimumHeight, dpiKey)
                                << " sizeHint 高=" << DpiUtil::scalePx(kBaseHeight, dpiKey);
}

void TextEditFull::resetWidgetSize()
{
    // 边缘宽度已按 DPI 换算（edgeMargin），故扣减 2 * edgeMargin
    textEdit->resize(width() - 2 * edgeMargin, height() - 2 * edgeMargin);
}

QString TextEditFull::toPlainText() const
{
    return textEdit->toPlainText();
}

void TextEditFull::clearText()
{
    textEdit->clear();
}

void TextEditFull::setSending(bool isSending)
{
    textEdit->setSending(isSending);
}
