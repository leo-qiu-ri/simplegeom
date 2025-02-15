#include <iostream>

#include "simplegeom/simplegeom.h"

int main() {
    simplegeom::PointGeo2 point(0.5, 0.5);
    simplegeom::PointGeo2 point2(2, 2);
    simplegeom::LineString<simplegeom::PointGeo2> line;
    line.emplace_back(0, 0);
    line.emplace_back(1, 1);

    double a = 0;
    std::cout << simplegeom::distance(point, point2) << std::endl;
    std::cout << simplegeom::distance(point2, line, simplegeom::ProjectionMode::kAccumulate).second << std::endl;
    std::cout << simplegeom::wkt_str(point2) << std::endl;
}