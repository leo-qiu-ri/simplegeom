#pragma once

#include "simplegeom/algorithm/closest_points.h"
#include "simplegeom/algorithm/convert.h"
#include "simplegeom/common.h"
#include "simplegeom/io.h"

namespace simplegeom {

/**
 * @brief 计算两个点之间的几何距离。
 *
 * 该函数用于计算两个点之间的几何距离。针对不同类型的地理点（如二维地理点 `PointGeo2` 或三维地理点 `PointGeo3`），
 * 函数会自动选择合适的距离计算策略。对于地理点类型，使用 Vincenty 算法计算距离；
 * 对于其他点类型，使用默认的几何距离计算方法。
 *
 * @tparam Point 点的类型。可以是普通二维或三维点，也可以是地理点类型（如 `PointGeo2` 或 `PointGeo3`）。
 * @param [in] p1 第一个点，类型为 `Point`。
 * @param [in] p2 第二个点，类型为 `Point`。
 * @return double 返回两个点之间的几何距离，类型为 `double`。
 *
 * @note 该函数会根据点的类型自动选择距离计算策略。对于地理点类型，使用 Vincenty 算法以提高计算精度。
 */
template <typename Point>
double distance(const Point &p1, const Point &p2) {
    if constexpr (std::is_same_v<Point, PointGeo2> || std::is_same_v<Point, PointGeo3>) {
        static const auto kVincentyStrategy = bg::strategy::distance::vincenty<>();
        return bg::distance(p1, p2, kVincentyStrategy);
    }
    return bg::distance(p1, p2);
}

/**
 * @brief 计算点到几何对象的距离。
 *
 * 该函数是一个通用模板函数，用于计算给定点到任意几何对象的最短距离。它依赖于 Boost.Geometry 库的 `bg::distance` 方法，
 * 支持多种几何对象类型，例如点、线段、多边形等。
 *
 * @tparam Geometry 几何对象的模板类型。需要是一个接受 `Point` 类型作为模板参数的模板类，例如 `Segment<Point>` 或
 * `Polygon<Point>`。
 * @tparam Point 点的类型。通常为二维或三维坐标点，例如 `Point2`或`PointGeo3`。
 * @param [in] point 要计算距离的点，类型为 `Point`。
 * @param [in] geometry 几何对象，类型为 `Geometry<Point>`，可以是线段、多边形等。
 * @return double 返回点到几何对象的最短距离，类型为 `double`。
 *
 * @note 该函数是模板实现，支持多种几何对象类型。实际计算由 Boost.Geometry 的 `bg::distance` 方法完成。
 */
template <template <typename> typename Geometry, typename Point>
double distance(const Point &point, const Geometry<Point> &geometry) {
    return bg::distance(point, geometry);
}

/**
 * @brief 计算点到折线的最短距离及相关投影距离。
 *
 * 该函数用于计算给定点到折线的最短距离，并可以根据投影模式计算累积距离。函数通过搜索框逐步缩小搜索范围，以优化计算效率。
 *
 * @tparam Point 点的类型，通常为二维或三维点。
 * @param [in] point 要计算距离的点。
 * @param [in] line 折线，由一系列点组成。
 * @param [in] mode 投影模式，决定是否计算累积距离。可能的值为
 * `ProjectionMode::kAccumulate`或`ProjectionMode::kSimple`。
 * @return std::pair<double, double> 返回一个包含两个值的 pair：
 *         - 第一个值表示点到折线的最短距离。
 *         - 第二个值表示点在线上的投影距离，具体含义取决于投影模式。
 */
template <typename Point>
std::pair<double, double> distance(const Point &point, const LineString<Point> &line, ProjectionMode mode) {
    static constexpr double kSearchBoxEdgeLength = 2000.;  // 初始搜索范围大小

    // 空线段
    if (line.empty()) {
        return std::make_pair(-1., 0.);
    } else if (line.size() < 2) {
        return std::make_pair(distance(point, line.front()), 0.);
    }

    size_t index = 0;
    double search_box_size = kSearchBoxEdgeLength;  // 如果线段与搜索框不相交，则跳过该线段

    Segment<Point> seg;
    for (size_t i = 0; i < line.size() - 1; ++i) {
        assign_segment(line.at(i), line.at(i + 1), seg);
        // 如果线段与搜索框不相交则跳过
        if (!bg::intersects(seg, create_box(point, search_box_size))) {
            continue;
        }

        // 如果线段与搜索框不相交，则跳过该线段
        if (auto d = distance(point, seg) * 2; d < search_box_size) {
            search_box_size = d;
            index = i;
        }
    }

    // 累积线段长度
    double project_distance = 0;
    if (mode == ProjectionMode::kAccumulate) {
        for (size_t i = 0; i < index; ++i) {
            project_distance += distance(line.at(i), line.at(i + 1));
        }
    }

    assign_segment(line.at(index), line.at(index + 1), seg);

    project_distance += distance(seg.first, closest_points(point, seg));

    return std::make_pair(bg::distance(point, seg), project_distance);
}

}  // namespace simplegeom
