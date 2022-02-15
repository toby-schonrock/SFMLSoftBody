#include <iostream>

#include "Point.hpp"
#include "Vector2.hpp"

constexpr float radius = 0.05F;

int main() {
    std::cout << Vec2(0,1) - Vec2(0,2) << '\n'; // [0, 1]
    std::cout << (Vec2(0,0) - Vec2(1,1)).mag() << '\n'; // 1.41421
    std::cout << Vec2(1,1).dot(Vec2(2,2)) << '\n'; // 4
    std::cout << Vec2(1,1).dot(Vec2(2,-2)) << '\n'; // 0
    std::cout << Vec2(2,2).norm() << '\n'; // 0

    Point p1(Vec2(3, 3), 1.0, radius);
    Point p2(Vec2(3, 3.2), 1.0, radius);
    Point::springHandler(p1, p2, 1, 8000, 100);
    std::cout << p1.f << '\n';
}