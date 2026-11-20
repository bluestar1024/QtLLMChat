#ifndef DPIUTIL_H
#define DPIUTIL_H

#pragma once

#include <QtCore/QMargins>
#include <QtCore/QSize>
#include <QtCore/Qt>
#include <QtCore/QtMath>

// 全项目统一的 DPI 换算基准：144 DPI（Windows 150% 缩放）屏上的物理像素。
// 各部件构造函数中写出的默认尺寸、边距、间距、最小高度都是该基准下的数值，
// 其它 DPI 一律按“基准值 × dpi / 144”绝对换算，不以“上一次 DPI”为基准逐次
// 叠乘：逐次换算每级都要取整，来回跨屏后各部件会累积误差（例如 44 → 29 →
// 44 之外的漂移），且中间过渡帧会污染基准。dpi <= 0 时按 144 基准处理。
namespace DpiUtil {

constexpr int kBaseDpi = 144;

// 目标 DPI 相对 144 基准的比例（dpi 非法时为 1，即基准本身）
inline qreal scaleOf(qreal dpi)
{
    return dpi > 0 ? dpi / qreal(kBaseDpi) : 1.0;
}

// 基准像素 → 目标 DPI 像素（尺寸至少为 minimum，避免异常 DPI 下控件消失；
// 边距/间距传 minimum=0）
inline int scalePx(int base, qreal dpi, int minimum = 1)
{
    return qMax(minimum, qRound(base * scaleOf(dpi)));
}

inline int scalePx(int base, int dpi, int minimum = 1)
{
    return scalePx(base, qreal(dpi), minimum);
}

inline QSize scaleSize(const QSize &base, qreal dpi)
{
    return QSize(scalePx(base.width(), dpi), scalePx(base.height(), dpi));
}

inline QMargins scaleMargins(const QMargins &base, qreal dpi)
{
    return QMargins(scalePx(base.left(), dpi, 0), scalePx(base.top(), dpi, 0),
                    scalePx(base.right(), dpi, 0), scalePx(base.bottom(), dpi, 0));
}

// 某 DPI 下实测/缓存的像素 → 144 基准像素：缓存的尺寸一律折算成基准值保存，
// 下次换算时再按目标 DPI 放大，跨屏往返不会累积取整误差
inline int toBasePx(int px, qreal dpi)
{
    return dpi > 0 ? qRound(px / scaleOf(dpi)) : px;
}

inline QSize toBaseSize(const QSize &px, qreal dpi)
{
    return QSize(toBasePx(px.width(), dpi), toBasePx(px.height(), dpi));
}

inline QMargins toBaseMargins(const QMargins &px, qreal dpi)
{
    return QMargins(toBasePx(px.left(), dpi), toBasePx(px.top(), dpi),
                    toBasePx(px.right(), dpi), toBasePx(px.bottom(), dpi));
}

// 从 fromDpi 下的像素换算到 toDpi：内部先归一化回 144 基准再放大（等价于
// fromDpi → toDpi 的比例，但基准固定为 144，语义上不依赖“上一次 DPI”）
inline int rescalePx(int px, qreal fromDpi, qreal toDpi, int minimum = 1)
{
    return scalePx(toBasePx(px, fromDpi), toDpi, minimum);
}

} // namespace DpiUtil

#endif // DPIUTIL_H