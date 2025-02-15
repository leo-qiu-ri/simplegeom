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
    static const double kGeographicFactor = 1. / 2. / 1e5;

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
Point closest_point(const Point &p, const Segment<Point> &seg) {
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
        seg.first = line.at(i);
        seg.second = line.at(i + 1);
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

    seg.first = line.at(index);
    seg.second = line.at(index + 1);

    Point closest = closest_point(point, seg);
    project_distance += distance(seg.first, closest);

    return std::make_pair(bg::distance(point, seg), project_distance);
}
}  // namespace simplegeom