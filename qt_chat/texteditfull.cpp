#include "texteditfull.h"
#include "appcontext.h"
#include "dpiutil.h"

#include <QtGui/QCursor>
#include <QtGui/QRegion>
#include <QtGui/QScreen>
#include <QtWidgets/QApplication>

namespace {
// chatInput 自身尺寸以 144 DPI（150% 缩放）屏为基准（DpiUtil::kBaseDpi）：最小
// 高度、高度 sizeHint、边缘宽度（15px 内边距）与边缘圆角（16px）均按
// dpi / 144 换算，让两档屏上物理大小一致；只有 1px 描边内缩不缩放（保证描边
// 仍为 1 像素）
constexpr int kBaseEdgeMargin = 15;
constexpr int kBaseEdgeRadius = 16;
constexpr int kBaseMinimumHeight = 80;
// 控件顶部为顶部胶囊让出的透明带：圆角框整体下移这么多，胶囊上半截落在这段
// 透明带里（骑在顶边框线上），圆角框与内部文本区尺寸不受影响（总高相应增加）
constexpr int kBaseTopInset = 5;
// 顶部高度调节胶囊（提示可拖动改变高度）：尺寸与命中区外扩量均以 144 DPI 为
// 基准按屏 DPI 换算；138×9 在 96 DPI 下为 92×6，与参考图一致
constexpr int kBaseResizeGripWidth = 138;
constexpr int kBaseResizeGripHeight = 9;
constexpr int kBaseResizeGripHitPadding = 4;
// 胶囊空闲描边向白混合的比例：描边比输入框当前边框色淡 25%（c + (255-c)*0.25），
// 聚焦态 #646464 -> #8B8B8B、未聚焦悬停态 #A8A8A8 -> #BEBEBE，两者都与
// #E0E0E0 的主体填充形成可见层次；悬停/拖动时描边改为边框原色
constexpr qreal kResizeGripIdleLighten = 0.25;
// 输入框失焦但鼠标在框内时的边框色：比失焦常态 #E0E0E0 深，作为可见的胶囊
// 承载轮廓；鼠标离开且胶囊隐藏后恢复 #E0E0E0
const QColor kUnfocusedPointerInsideBorder(168, 168, 168);
// chatInput 在 144 DPI 屏（基准）下的默认高度：文本区高度取 TextEdit 的项目
// 约定常量 TextEdit::kBaseTextHeight（190），加上下边缘各 15，再加顶部透明带 5；
// 96 DPI 下换算为 150（文本区 127 = 150 - 2 * 10 - 3）
constexpr int kBaseHeight = TextEdit::kBaseTextHeight + 2 * kBaseEdgeMargin + kBaseTopInset;

// 胶囊空闲描边：以输入框当前边框色为基准向白混合，跟随边框色动画
QColor lightenResizeGripBorder(const QColor &borderColor)
{
    return QColor::fromRgbF(borderColor.redF() + (1.0 - borderColor.redF()) * kResizeGripIdleLighten,
                            borderColor.greenF() + (1.0 - borderColor.greenF()) * kResizeGripIdleLighten,
                            borderColor.blueF() + (1.0 - borderColor.blueF()) * kResizeGripIdleLighten,
                            borderColor.alphaF());
}
} // namespace

TextEditFull::TextEditFull(AppContext *appContext, QWidget *parent)
    : QWidget(parent),
      appContext(appContext),
      backgroundColorIsLight(false),
      appliedDpi(-1),
      appliedFontPixelSize(-1),
      logNextResize(false),
      edgeMargin(kBaseEdgeMargin),
      edgeRadius(kBaseEdgeRadius),
      topInset(kBaseTopInset),
      pointerInside(false),
      gripHovered(false),
      gripDragging(false)
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
    animatedBorderTarget = bColor;

    animationBackgroundColor = new QPropertyAnimation(this, "backgroundColor");
    animationBackgroundColor->setDuration(400);
    animationBackgroundColor->setEasingCurve(QEasingCurve::OutQuad);

    animationBorderColor = new QPropertyAnimation(this, "borderColor");
    animationBorderColor->setDuration(400);
    animationBorderColor->setEasingCurve(QEasingCurve::OutQuad);

    setMouseTracking(true);

    // 胶囊显隐需要知道"输入框是否有焦点"与"鼠标是否在输入框区域内"：焦点
    // 事件落在内层 TextEdit 上；鼠标进出文本区时事件落在其 viewport 上，两者
    // 都通过事件过滤转发到 updatePointerState
    textEdit->installEventFilter(this);
    textEdit->viewport()->installEventFilter(this);

    // 构造时窗可能尚未定位到实际屏（screen() 常为主屏），这里先按当前可取得
    // 的屏 DPI 换算一次，窗口落到其他 DPI 的屏后由 MainWindow 的 DPI 缩放链
    // 与 event() 的换屏兜底再次校正
    applyDpiScale(0);
}

TextEditFull::~TextEditFull() { }

bool TextEditFull::event(QEvent *e)
{
    // 拖动中被窗口失活/隐藏/夺走鼠标抓取打断时立即收尾并回写高度基准，
    // 避免拖动状态悬挂、胶囊停在加深态
    if (e->type() == QEvent::WindowDeactivate || e->type() == QEvent::Hide
            || e->type() == QEvent::UngrabMouse) {
        if (gripDragging)
            finishResizeGripDrag();
    }
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
    const QPoint pos = e->position().toPoint();
    if (gripDragging) {
        // 拖动胶囊：把全局纵坐标交给 MainWindow 调整 splitter 高度（鼠标可能
        // 已移出本控件，事件仍由隐式抓取送达）
        emit resizeGripMoved(qRound(e->globalPosition().y()));
        e->accept();
        return;
    }
    // 正常情况下 pointerInside 已由 enterEvent 置位；这里兜底并刷新悬停状态
    pointerInside = true;
    updateResizeGripHover(pos);
    QWidget::mouseMoveEvent(e);
    e->ignore();
}

void TextEditFull::mousePressEvent(QMouseEvent *e)
{
    if (e->button() == Qt::LeftButton && isResizeGripVisible()
            && resizeGripHitRect().contains(e->position().toPoint())) {
        gripDragging = true;
        gripHovered = true;
        update();
        emit resizeGripPressed(qRound(e->globalPosition().y()));
        // 吞掉按下事件：避免松手落到 MainWindow 的"点击输入框外部"判断
        e->accept();
        return;
    }
    QWidget::mousePressEvent(e);
    e->ignore();
}

void TextEditFull::mouseReleaseEvent(QMouseEvent *e)
{
    if (gripDragging && e->button() == Qt::LeftButton) {
        finishResizeGripDrag();
        e->accept();
        return;
    }
    QWidget::mouseReleaseEvent(e);
    e->ignore();
}

void TextEditFull::enterEvent(QEnterEvent *e)
{
    QWidget::enterEvent(e);
    updatePointerState();
}

void TextEditFull::leaveEvent(QEvent *e)
{
    QWidget::leaveEvent(e);
    updatePointerState();
}

bool TextEditFull::eventFilter(QObject *watched, QEvent *e)
{
    if (watched == textEdit) {
        // 焦点在输入框内时即使鼠标不在也显示胶囊；失焦后按鼠标位置决定
        if (e->type() == QEvent::FocusIn || e->type() == QEvent::FocusOut)
            updatePointerState();
    } else if (watched == textEdit->viewport()) {
        // 鼠标进出文本区：Enter/Leave 在父子控件间的传播顺序不可靠，
        // 统一用鼠标全局位置重算，避免胶囊闪烁或残留
        if (e->type() == QEvent::Enter || e->type() == QEvent::Leave)
            updatePointerState();
    }
    return QWidget::eventFilter(watched, e);
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

QRect TextEditFull::boxRect() const
{
    // 1px 描边内缩不缩放；顶部再让出 topInset 透明带，胶囊上半截落在带内
    return rect().adjusted(1, 1 + topInset, -1, -1);
}

QRect TextEditFull::resizeGripRect() const
{
    const int dpi = appliedDpi > 0 ? appliedDpi : DpiUtil::kBaseDpi;
    const int gripWidth = DpiUtil::scalePx(kBaseResizeGripWidth, dpi);
    const int gripHeight = DpiUtil::scalePx(kBaseResizeGripHeight, dpi);
    const int x = (width() - gripWidth) / 2;
    // 垂直骑在顶边框线上：高度为奇数时正中一行就是描边所在行
    int y = boxRect().top() - (gripHeight - 1) / 2;
    if (y < 0)
        y = 0;
    return QRect(x, y, gripWidth, gripHeight);
}

QRect TextEditFull::resizeGripHitRect() const
{
    const int dpi = appliedDpi > 0 ? appliedDpi : DpiUtil::kBaseDpi;
    const int padding = DpiUtil::scalePx(kBaseResizeGripHitPadding, dpi, 0);
    QRect hit = resizeGripRect().adjusted(-padding, -padding, padding, padding);
    hit.setTop(qMax(0, hit.top()));
    // 下侧不侵入文本区：主布局上边距即文本区顶边
    hit.setBottom(qMin(hit.bottom(), mainHLayout->contentsMargins().top() - 1));
    return hit;
}

bool TextEditFull::isResizeGripVisible() const
{
    return gripDragging || pointerInside || inputHasFocus();
}

bool TextEditFull::inputHasFocus() const
{
    if (textEdit->hasFocus())
        return true;
    const QWidget *focus = QApplication::focusWidget();
    return focus && textEdit->isAncestorOf(focus);
}

void TextEditFull::updatePointerState()
{
    // 鼠标可能在任意子控件上：统一用全局光标位置判断是否仍在本控件矩形内
    // （光标进入子控件时父控件收到的是 leaveEvent，不能只依赖 enter/leave）
    const QPoint localPos = mapFromGlobal(QCursor::pos());
    pointerInside = isVisible() && rect().contains(localPos);
    updateBorderColorTarget();
    updateResizeGripHover(pointerInside ? localPos : QPoint(-1, -1));
    update();
}

void TextEditFull::updateResizeGripHover(const QPoint &pos)
{
    if (gripDragging)
        return;
    const bool hovered = isResizeGripVisible() && resizeGripHitRect().contains(pos);
    if (hovered == gripHovered)
        return;
    gripHovered = hovered;
    if (hovered)
        setCursor(Qt::SplitVCursor);
    else
        unsetCursor();
    update();
}

void TextEditFull::finishResizeGripDrag()
{
    if (!gripDragging)
        return;
    gripDragging = false;
    // 先通知 MainWindow 回写高度基准，再按当前鼠标位置刷新悬停与显隐
    emit resizeGripReleased();
    updatePointerState();
}

void TextEditFull::paintEvent(QPaintEvent *e)
{
    Q_UNUSED(e);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    const QRect box = boxRect();
    QPainterPath boxPath;
    boxPath.addRoundedRect(QRectF(box), edgeRadius, edgeRadius);
    // 主体整块填充：胶囊露出圆角框的部分随后另填（主体色带透明度时不叠加两次）
    painter.fillPath(boxPath, bgColor);

    if (!isResizeGripVisible()) {
        painter.setPen(QPen(bColor, 1));
        painter.setBrush(Qt::NoBrush);
        painter.drawPath(boxPath);
        painter.end();
        return;
    }

    const QRect grip = resizeGripRect();
    const qreal gripRadius = (grip.height() - 1) / 2.0;
    QPainterPath gripPath;
    gripPath.addRoundedRect(QRectF(grip), gripRadius, gripRadius);

    // 胶囊上半截在圆角框外：按主体色补齐（裁剪到圆角框顶线以上，路径外的
    // 下半截已由主体填充覆盖）
    if (grip.top() < box.top()) {
        painter.save();
        painter.setClipRect(QRectF(QPointF(grip.left(), grip.top()),
                                   QPointF(grip.right() + 1, box.top())));
        painter.fillPath(gripPath, bgColor);
        painter.restore();
    }

    // 顶边框线在胶囊宽度范围断开（左右各留 1px 抗锯齿余量）
    QRegion clip(rect());
    clip -= QRegion(grip.adjusted(-1, 0, 1, 1));
    painter.save();
    painter.setClipRegion(clip);
    painter.setPen(QPen(bColor, 1));
    painter.setBrush(Qt::NoBrush);
    painter.drawPath(boxPath);
    painter.restore();

    // 胶囊描边：悬停/拖动用边框当前色（与圆角框描边一致），否则用比边框色
    // 更淡的描边；均随边框色动画逐帧取值。内缩半像素保证 1px 清晰
    const QColor gripBorder = (gripHovered || gripDragging)
            ? bColor : lightenResizeGripBorder(bColor);
    QPainterPath gripOutline;
    gripOutline.addRoundedRect(QRectF(grip).adjusted(0.5, 0.5, -0.5, -0.5),
                               (grip.height() - 2) / 2.0, (grip.height() - 2) / 2.0);
    painter.setPen(QPen(gripBorder, 1));
    painter.setBrush(Qt::NoBrush);
    painter.drawPath(gripOutline);

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

        // 聚焦态边框固定为边框原色，与鼠标位置无关
        updateBorderColorTarget();
    }
}

void TextEditFull::backgroundColorShowDark()
{
    if (backgroundColorIsLight) {
        backgroundColorIsLight = false;
        animationBackgroundColor->setStartValue(appContext->textEditFullBGTColor());
        animationBackgroundColor->setEndValue(appContext->textEditFullBGColor());
        animationBackgroundColor->start();

        // 失焦态边框分两档：鼠标在框内 #A8A8A8，否则 #E0E0E0
        updateBorderColorTarget();
    }
}

QColor TextEditFull::borderTargetColor() const
{
    if (backgroundColorIsLight)
        return appContext->textEditFullBColor();
    // 拖动中鼠标可能已移出控件，仍视为在输入框内：否则胶囊描边会退回与
    // 填充同色，拖动途中轮廓消失
    if (pointerInside || gripDragging)
        return kUnfocusedPointerInsideBorder;
    return appContext->textEditFullBTColor();
}

void TextEditFull::updateBorderColorTarget()
{
    const QColor target = borderTargetColor();
    if (target == animatedBorderTarget)
        return;
    animatedBorderTarget = target;
    // 从当前动画中间值接着走：快速进出/切换聚焦时颜色连续，不从端点跳变
    animationBorderColor->stop();
    animationBorderColor->setStartValue(bColor);
    animationBorderColor->setEndValue(target);
    animationBorderColor->start();
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
    topInset = DpiUtil::scalePx(kBaseTopInset, dpiKey, 0);
    // 圆角框下移 topInset：上边距加上这段透明带，文本区高度不变（总高相应增加）
    mainHLayout->setContentsMargins(edgeMargin, edgeMargin + topInset, edgeMargin, edgeMargin);
    update();

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
    // 边缘宽度与顶部透明带已按 DPI 换算（edgeMargin / topInset）
    textEdit->resize(width() - 2 * edgeMargin, height() - 2 * edgeMargin - topInset);
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
