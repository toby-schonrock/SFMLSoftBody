#include <cmath>
#include <chrono>
#include <thread>
#include <iostream>

#include <SFML/Graphics.hpp>
#include <Vector2.hpp>
#include <Matrix.hpp>

constexpr double vsScale = 100.0;

sf::Vector2f visualize(const Vec2& v);

class Point {
public:
    Vec2 pos;
    Vec2 vel{0, 0}; // set to 0,0
    Vec2 f;
    sf::CircleShape shape;
    double mass = 1.0;
    double radius;

    Point() {}

    Point(Vec2 pos_, double mass_, double radius_) : pos(pos_), mass(mass_), radius(radius_) {
        shape = sf::CircleShape(static_cast<float>(radius * vsScale));
        shape.setFillColor(sf::Color::Green);
        shape.setPosition(visualize(pos));
        shape.setOrigin(visualize(Vec2(radius, radius)));
    }

    void update(double deltaTime, sf::RenderWindow& window, double gravity) {
        f += Vec2(0, 1) * (gravity * mass); // add gravity to the force
        vel += (f * deltaTime) / mass; // euler integration could be improved
        pos += vel * deltaTime;
        shape.setPosition(visualize(pos));
        window.draw(shape);
        // std::cout << deltaTime << '\n';
        // std::cout << pos << '\n';
        f = Vec2();
    }
};

sf::Vector2f visualize(const Vec2& v) {
    return sf::Vector2f(static_cast<float>(v.x * vsScale), static_cast<float>(v.y * vsScale));
}

void springHandler(Point& p1, Point& p2, double stableScale) {
    Vec2 diff = p1.pos - p2.pos;
}

void simFrame(Matrix<Point>& points) {
    static constexpr double r2 = 1.4142857;
    for (int x = 0; x < points.sizeX; x++) {
        for (int y = 0; y < points.sizeY; y++) {
            Point& p = points(x, y);
            if (x < points.sizeX - 1) {  // to the right
                if (y < points.sizeY - 1) { // to the right and above
                    springHandler(p, points(x + 1, y + 1), r2);
                }
                springHandler(p, points(x + 1, y), 1.0);
            }
            if (y < points.sizeY - 1) { // above
                if (x > 0) { // to the left and above
                    springHandler(p, points(x - 1, y), r2);
                }
                springHandler(p, points(x - 1, y + 1), 1.0);
            }
        }
    }
}

int main() {
    constexpr Vec2 scale(1, 2);
    constexpr Vec2 gap(0.2, 0.2);
    constexpr Vec2I size(static_cast<int>(scale.x / gap.x), static_cast<int>(scale.y / gap.y));
    constexpr Vec2 simPos(1, 1);
    constexpr double radius = 0.00005;
    constexpr double gravity = 9.8;

    // mass is 1 kg per point
    // 1.. is one meter

    std::cout << size;
 
    sf::RenderWindow window(sf::VideoMode(1920, 1080), "Soft Body Simulation"); //, sf::Style::Fullscreen);

    Matrix<Point> points(size.x, size.y);
    for (int x = 0; x < size.x; x++) {
        for (int y = 0; y < size.y; y++) {
            points(x, y) = Point(Vec2(x * gap.x, y * gap.y) + simPos, radius, 1.0);
        } 
    }

    // Point p(Vec2(0, 0), radius, 1.0);

    std::chrono::_V2::system_clock::time_point last = std::chrono::high_resolution_clock::now();

    while (window.isOpen()) {
        std::chrono::_V2::system_clock::time_point start = std::chrono::high_resolution_clock::now();

        // clear poll events for sfml
        sf::Event event;
        while (window.pollEvent(event)) {
             if (event.type == sf::Event::Closed) window.close();
        }




        std::chrono::nanoseconds deltatime = std::chrono::high_resolution_clock::now() - last;
        last = std::chrono::high_resolution_clock::now();
        window.clear();
        // std::cout << nanos.count() << '\n';
        simFrame(points);
        for (int x = 0; x < points.sizeX; x++) {
            for (int y = 0; y < points.sizeY; y++) {
                points(x, y).update(static_cast<double>(deltatime.count()) / 1e9, window, gravity);
            }
        }
        // p.update(static_cast<double>(nanos.count()) / 1000000.0, window, gravity);




        window.display();
        std::chrono::nanoseconds nanos = std::chrono::high_resolution_clock::now() - start;
        // std::cout << "elapsed time: " << nanos.count() << "ns\n";
        while ((nanos.count() < 1'000'000)) {
            nanos = std::chrono::high_resolution_clock::now() - start;
        }
    }

    return 0;
}