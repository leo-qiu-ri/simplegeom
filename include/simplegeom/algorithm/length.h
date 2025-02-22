#pragma once

#include "simplegeom/common.h"
#include "simplegeom/io.h"

namespace simplegeom {

/**
 * @brief 计算几何对象的长度或周长。
 *
 * 该函数用于计算几何对象（如线串、多边形等）的长度或周长。
 * 通过调用Boost.Geometry库的`bg::length`函数，根据几何对象的点类型，
 * 自动选择合适的方式（如欧几里得距离）进行计算。
 *
 * @tparam Geometry 几何对象的模板类型，例如`LineString`或`Polygon`。
 * @tparam Point 几何对象中使用的点类型。
 * @param[in] geometry 待计算长度的几何对象，类型为`Geometry<Point>`。
 * @return double 返回几何对象的长度或周长，类型为`double`。
 */
template <template <typename> typename Geometry, typename Point>
double length(const Geometry<Point>& geometry) {
    return bg::length(geometry);
}

}  // namespace simplegeom
