#include <chrono>
#include <iostream>

#include "simplegeom/simplegeom.h"

int main() {
    simplegeom::Point2 point(0.5, 0.5);
    simplegeom::Point2 point2(2, 2);
    simplegeom::LineString<simplegeom::Point2> line;
    line.emplace_back(0, 0);
    line.emplace_back(1, 1);
    line.emplace_back(2, 2);

    double a = 0;
    auto start = std::chrono::high_resolution_clock::now();
    std::cout << simplegeom::distance(point, point2) << std::endl;
    std::cout << "distance cost: "
              << std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() -
                                                                       start)
                     .count()
              << "us." << std::endl;
    std::cout << simplegeom::distance(point2, line, simplegeom::ProjectionMode::kAccumulate).second << std::endl;
    std::cout << "proj distance cost: "
              << std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() -
                                                                       start)
                     .count()
              << "us." << std::endl;
    std::cout << simplegeom::wkt_str(point2) << std::endl;
}