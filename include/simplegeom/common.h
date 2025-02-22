#pragma once

#include <boost/geometry.hpp>
#include <boost/geometry/geometries/geometries.hpp>

namespace simplegeom {
namespace bg = boost::geometry;

template <size_t N>
using PointX = bg::model::point<double, N, bg::cs::cartesian>;
template <size_t N>
using PointGeoX = bg::model::point<double, 2, bg::cs::geographic<bg::degree>>;
using Point2 = PointX<2>;
using Point3 = PointX<3>;
using PointGeo2 = PointGeoX<2>;
using PointGeo3 = PointGeoX<3>;

template <typename Point>
using LineString = bg::model::linestring<Point>;

template <typename Point>
using Box = bg::model::box<Point>;

template <typename Point>
using Polygon = bg::model::polygon<Point>;

template <typename Point>
using Segment = bg::model::segment<Point>;

/// @brief 投影模式
enum class ProjectionMode { kSimple, kAccumulate };

constexpr double kEarthA = 6378137.0;                                       ///< Earth's semi-major axis
constexpr double kEarthB = 6356752.3142;                                    ///< Earth's semi-minor axis
constexpr double kEarthE2 = 1 - (kEarthB * kEarthB) / (kEarthA * kEarthA);  ///< Earth's first eccentricity squared

}  // namespace simplegeom