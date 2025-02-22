#pragma once

#include "simplegeom/common.h"
#include "simplegeom/io.h"

namespace simplegeom {

static constexpr double kToRad = M_PI / 180.0;

/**
 * @brief 将地理坐标点转换为局部坐标系中的点。
 *
 * 该函数用于将两个地理坐标点（origin和target）之间的差异转换为局部坐标系中的点。
 * 局部坐标系的原点是origin，target是目标点。根据地球的椭球体模型，计算两个点之间的
 * 经度差、纬度差和高程差（如果是三维情况），并将它们转换为局部坐标系中的坐标。
 * 支持二维（2D）和三维（3D）点的转换。
 *
 * @tparam N 点的维度，可以是2（二维平面）或3（三维空间）。
 * @param[in] origin 参考点（局部坐标系的原点），类型为PointGeoX<N>。
 * @param[in] target 目标点，类型为PointGeoX<N>。
 * @return PointX<N> 返回局部坐标系中的点，类型为PointX<N>。
 */
template <size_t N>
PointX<N> transform(const PointGeoX<N> &origin, const PointGeoX<N> &target) {
    static_assert(N == 2 || N == 3, "Only support 2D or 3D point");
    const double sin_lat = std::sin(bg::get<1>(origin) * kToRad);
    const double cos_lat = std::cos(bg::get<1>(origin) * kToRad);

    // 计算地球的曲率半径（子午圈曲率半径rn和卯酉圈曲率半径rm）
    double rn = kEarthA / std::sqrt(1.0 - kEarthE2 * sin_lat * sin_lat);
    double rm = rn * (1.0 - kEarthE2) / (1.0 - kEarthE2 * sin_lat * sin_lat);

    const double dlon = (bg::get<0>(target) - bg::get<0>(origin)) * kToRad;
    const double dlat = (bg::get<1>(target) - bg::get<1>(origin)) * kToRad;

    if constexpr (N == 2) {
        return PointX<2>(rm * dlat, rn * dlon * cos_lat);
    } else {
        // 对于三维情况，考虑高程对曲率半径的影响
        rn += bg::get<2>(origin);
        rm += bg::get<2>(origin);

        const double dh = bg::get<2>(target) - bg::get<2>(origin);

        return PointX<3>(rm * dlat, rn * dlon * cos_lat, -dh);
    }
}

/**
 * @brief 将地理坐标的线串（LineString）转换为局部坐标系中的线串。
 *
 * 该函数用于将地理坐标的线串（由多个点组成）相对于参考点（origin）转换为局部坐标系中的线串。
 * 根据地球的椭球体模型，计算每个点与参考点的经度差、纬度差和高程差（如果是三维情况），
 * 并将它们投影到局部坐标系中。支持二维（2D）和三维（3D）点的转换。
 *
 * @tparam N 点的维度，可以是2（二维平面）或3（三维空间）。
 * @param[in] origin 参考点（局部坐标系的原点），类型为PointGeoX<N>。
 * @param[in] line 需要转换的目标线串，类型为LineString<PointGeoX<N>>。
 * @return LineString<PointX<N>> 返回局部坐标系中的线串，类型为LineString<PointX<N>>。
 */
template <size_t N>
LineString<PointX<N>> transform(const PointGeoX<N> &origin, const LineString<PointGeoX<N>> &line) {
    static_assert(N == 2 || N == 3, "Only support 2D or 3D point");
    const double sin_lat = std::sin(bg::get<1>(origin) * kToRad);
    const double cos_lat = std::cos(bg::get<1>(origin) * kToRad);

    // 计算地球的曲率半径（子午圈曲率半径rn和卯酉圈曲率半径rm）
    double rn = kEarthA / std::sqrt(1.0 - kEarthE2 * sin_lat * sin_lat);
    double rm = rn * (1.0 - kEarthE2) / (1.0 - kEarthE2 * sin_lat * sin_lat);

    auto res = LineString<PointX<N>>();
    res.reserve(line.size());

    if constexpr (N == 2) {
        for (const auto &target : line) {
            const double dlon = (bg::get<0>(target) - bg::get<0>(origin)) * kToRad;
            const double dlat = (bg::get<1>(target) - bg::get<1>(origin)) * kToRad;

            res.emplace_back(PointX<2>(rm * dlat, rn * dlon * cos_lat));
        }
    } else {
        // 对于三维情况，考虑高程对曲率半径的影响
        rn += bg::get<2>(origin);
        rm += bg::get<2>(origin);

        for (const auto &target : line) {
            const double dlon = (bg::get<0>(target) - bg::get<0>(origin)) * kToRad;
            const double dlat = (bg::get<1>(target) - bg::get<1>(origin)) * kToRad;
            const double dh = bg::get<2>(target) - bg::get<2>(origin);

            res.emplace_back(PointX<3>(rm * dlat, rn * dlon * cos_lat, -dh));
        }
    }

    return res;
}

}  // namespace simplegeom