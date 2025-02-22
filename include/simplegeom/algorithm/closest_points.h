#pragma once

#include "simplegeom/common.h"
#include "simplegeom/io.h"

namespace simplegeom {
/**
 * @brief 计算点到线段上的最近点。
 *
 * 该函数用于计算给定点到线段上的最近点。支持二维和三维点，基于向量投影公式计算最近点的坐标。
 * 如果点是三维的，函数会自动处理 z 坐标；如果是二维的，则忽略 z 坐标。
 *
 * @tparam Point 点的类型。可以是二维或三维点，需要支持 Boost.Geometry 的坐标访问接口。
 * @param [in] p 目标点，类型为 `Point`，表示需要计算最近点的点。
 * @param [in] seg 线段，类型为 `Segment<Point>`，由两个端点组成。
 * @return Point 返回线段上的最近点，类型为 `Point`。
 *
 * @note 该函数通过向量投影公式计算最近点，支持二维和三维点的处理。如果线段长度为 0，则返回线段的起点。如果线段长度为
 * 1，则返回线段的终点。
 */
template <typename Point>
Point closest_points(const Point &p, const Segment<Point> &seg) {
    constexpr size_t dim = bg::dimension<Point>::value;  // 点的维度
    static_assert(dim == 2 || dim == 3, "Only support 2D or 3D point");

    // 提取线段的起点 `a` 和终点 `b`
    const Point &a = seg.first;   // 线段的起点
    const Point &b = seg.second;  // 线段的终点

    // 提取点 `a`、点 `b` 和点 `p` 的坐标
    double ax = bg::get<0>(a), ay = bg::get<1>(a);  // 起点 `a` 的 x 和 y 坐标
    double bx = bg::get<0>(b), by = bg::get<1>(b);  // 终点 `b` 的 x 和 y 坐标
    double px = bg::get<0>(p), py = bg::get<1>(p);  // 目标点 `p` 的 x 和 y 坐标

    // 计算向量 AB 和 AP
    double abx = bx - ax, aby = by - ay;  // 向量 AB 的分量，表示从 `a` 到 `b` 的方向
    double apx = px - ax, apy = py - ay;  // 向量 AP 的分量，表示从 `a` 到 `p` 的方向

    // 根据投影比例 t 计算最近点的坐标
    // 公式：最近点 = A + t * AB
    if constexpr (dim > 2) {
        double abz = bg::get<2>(b) - bg::get<2>(a);  // 向量 AB 的 z 分量
        double apz = bg::get<2>(p) - bg::get<2>(a);  // 向量 AP 的 z 分量

        // 计算点 `p` 在向量 AB 上的投影比例 t
        // 公式：t = (AP · AB) / (AB · AB)，其中 · 表示点积
        double t = (apx * abx + apy * aby + apz * abz) / (abx * abx + aby * aby + abz * abz);
        // 处理投影比例 t 的特殊情况：
        // 1. 如果 t 是 NaN（例如线段长度为 0），则设 t 为 0。
        // 2. 将 t 限制在 [0, 1] 范围内，确保投影点在线段上。
        t = std::isnan(t) ? 0. : std::max(0., std::min(1., t));

        return Point(bg::get<0>(a) + t * abx, bg::get<1>(a) + t * aby, bg::get<2>(a) + t * abz);
    } else {
        double t = (apx * abx + apy * aby) / (abx * abx + aby * aby);
        t = std::isnan(t) ? 0. : std::max(0., std::min(1., t));

        return Point(bg::get<0>(a) + t * abx, bg::get<1>(a) + t * aby);
    }
}

}  // namespace simplegeom