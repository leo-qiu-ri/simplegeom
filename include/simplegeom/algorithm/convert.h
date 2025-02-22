#pragma once

#include "simplegeom/common.h"
#include "simplegeom/io.h"

namespace simplegeom {

/**
 * @brief 创建一个以给定点为中心的正方形（二维）或立方体（三维）边界框。
 *
 * 该函数用于生成一个以指定点为中心、边长为 `edge_length` 的正方形（二维）或立方体（三维）边界框。
 * 如果点的类型是地理点（`PointGeo2` 或 `PointGeo3`），则边长会自动调整以适应地理坐标的缩放比例。
 * 支持二维和三维点，点类型需满足 Boost.Geometry 的坐标访问接口。
 *
 * @tparam Point 点的类型。可以是二维或三维点，也可以是地理点（如 `PointGeo2` 或 `PointGeo3`）。
 * @param [in] center_point 边界框的中心点，类型为 `Point`。
 * @param [in] edge_length 边界框的边长，类型为 `double`。
 * @return Box<Point> 返回生成的边界框，类型为 `Box<Point>`，表示一个矩形（二维）或立方体（三维）。
 *
 * @note 该函数会根据点的类型和维度自动调整边界框的生成逻辑。对于地理点，边长会被缩放以适应地理坐标的缩放比例。
 */
template <typename Point>
Box<Point> create_box(const Point &center_point, double edge_length) {
    static constexpr double kGeographicFactor = 1. / 2. / 1e5;

    constexpr size_t dim = bg::dimension<Point>::value;  // 点的维度
    static_assert(dim == 2 || dim == 3, "Only support 2D or 3D point");

    if constexpr (std::is_same_v<Point, PointGeo2> || std::is_same_v<Point, PointGeo3>) {
        edge_length *= kGeographicFactor;
    }

    if constexpr (dim > 2) {
        return Box<Point>(Point(bg::get<0>(center_point) - edge_length, bg::get<1>(center_point) - edge_length,
                                bg::get<2>(center_point) - edge_length),
                          Point(bg::get<0>(center_point) + edge_length, bg::get<1>(center_point) + edge_length,
                                bg::get<2>(center_point) + edge_length));
    } else {
        return Box<Point>(Point(bg::get<0>(center_point) - edge_length, bg::get<1>(center_point) - edge_length),
                          Point(bg::get<0>(center_point) + edge_length, bg::get<1>(center_point) + edge_length));
    }
}

template <typename Point>
inline void assign_segment(const Point &p1, const Point &p2, Segment<Point> &seg) {
    seg.first = p1;
    seg.second = p2;
}

}  // namespace simplegeom