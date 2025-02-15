#pragma once

#include <sstream>

#include "simplegeom/common.h"

namespace simplegeom {
/**
 * @brief 设置 WKT（Well-Known Text）格式输出时的精度。
 *
 * 该函数用于根据点的类型设置 `std::stringstream` 的输出精度。对于地理点（如 `PointGeo2` 或 `PointGeo3`），
 * 设置为 7 位小数精度；对于其他点，设置为 2 位小数精度。WKT
 * 是一种用于表示几何对象的文本格式，常用于地理信息系统（GIS）中。
 *
 * @tparam Point 点的类型。可以是普通二维或三维点，也可以是地理点（如 `PointGeo2` 或 `PointGeo3`）。
 * @param [in,out] ss 标准字符串流对象，用于设置输出精度，类型为 `std::stringstream &`。
 *
 * @note 该函数会根据点的类型自动设置适当的输出精度，以确保 WKT 格式的准确性和清晰性。
 */
template <typename Point>
void wkt_precision(std::stringstream &ss) {
    if (std::is_same_v<Point, PointGeo2> || std::is_same_v<Point, PointGeo3>) {
        ss << std::fixed << std::setprecision(7);
    } else {
        ss << std::fixed << std::setprecision(2);
    }
}

/**
 * @brief 将点转换为 WKT（Well-Known Text）格式的字符串。
 *
 * 该函数用于将给定点转换为 WKT 格式的字符串。WKT 是一种用于表示几何对象的文本格式，常用于地理信息系统（GIS）中。
 * 函数会根据点的类型自动设置输出精度，以确保 WKT 格式的准确性和清晰性。
 *
 * @tparam Point 点的类型。可以是普通二维或三维点，也可以是地理点（如 `PointGeo2` 或 `PointGeo3`）。
 * @param [in] point 要转换的点，类型为 `Point`。
 * @return std::string 返回转换后的 WKT 格式字符串，类型为 `std::string`。
 *
 * @note 该函数会根据点的类型自动设置适当的输出精度，以确保 WKT 格式的准确性和清晰性。
 */
template <typename Point>
std::string wkt_str(const Point &point) {
    std::stringstream ss;
    wkt_precision<Point>(ss);
    ss << bg::wkt(point);
    return ss.str();
}

/**
 * @brief 将几何对象转换为 WKT（Well-Known Text）格式的字符串。
 *
 * 该函数用于将给定几何对象转换为 WKT 格式的字符串。WKT 是一种用于表示几何对象的文本格式，常用于地理信息系统（GIS）中。
 * 函数会根据点的类型自动设置输出精度，以确保 WKT 格式的准确性和清晰性。
 *
 * @tparam Geometry 几何对象的模板类型。需要是一个接受 `Point` 类型作为模板参数的模板类，例如 `Segment<Point>` 或
 * `Polygon<Point>`。
 * @tparam Point 点的类型。通常为二维或三维坐标点，例如 `Point2`或`PointGeo3`。
 * @param [in] geometry 要转换的几何对象，类型为 `Geometry<Point>`。
 * @return std::string 返回转换后的 WKT 格式字符串，类型为 `std::string`。
 *
 * @note 该函数会根据点的类型自动设置适当的输出精度，以确保 WKT 格式的准确性和清晰性。
 */
template <template <typename> typename Geometry, typename Point>
std::string wkt_str(const Geometry<Point> &geometry) {
    std::stringstream ss;
    wkt_precision<Point>(ss);
    ss << bg::wkt(geometry);
    return ss.str();
}

}  // namespace simplegeom