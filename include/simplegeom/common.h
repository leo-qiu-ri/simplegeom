#pragma once

#include <boost/geometry.hpp>
#include <boost/geometry/geometries/geometries.hpp>

namespace simplegeom {
namespace bg = boost::geometry;

using Point2 = bg::model::point<double, 2, bg::cs::cartesian>;
using Point3 = bg::model::point<double, 3, bg::cs::cartesian>;
using PointGeo2 = bg::model::point<double, 2, bg::cs::geographic<bg::degree>>;
using PointGeo3 = bg::model::point<double, 3, bg::cs::geographic<bg::degree>>;

template <typename Point>
using LineString = bg::model::linestring<Point>;

template <typename Point>
using Box = bg::model::box<Point>;

template <typename Point>
using Polygon = bg::model::polygon<Point>;

template <typename Point>
using Segment = bg::model::segment<Point>;

enum class ProjectionMode { kSimple, kAccumulate };

}  // namespace simplegeom