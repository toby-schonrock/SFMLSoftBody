#define _USE_MATH_DEFINES
#include <cmath>
#include <string>
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

    void draw(sf::RenderWindow& window) {
            shape.setPosition(visualize(pos));
            window.draw(shape);
    }

    void update(double deltaTime, double gravity) {
        f += Vec2(0, 1) * (gravity * mass); // add gravity to the force
        vel += (f * deltaTime) / mass; // euler integration could be improved
        pos += vel * deltaTime;
        // std::cout << deltaTime << '\n';
        // std::cout << pos << '\n';
        f = Vec2();
    }

    void polyColHandler() {
            if (pos.y > 7) {
                vel.y *= -1;
                pos.y = 7;
            }
        }
};

sf::Vector2f visualize(const Vec2& v) {
    return sf::Vector2f(static_cast<float>(v.x * vsScale), static_cast<float>(v.y * vsScale));
}

void springHandler(Point& p1, Point& p2, double stableScale) {
    static constexpr double stablePoint = 0.2;
    static constexpr double springConst = 300;
    static constexpr double dampFact = 2;

    Vec2 diff = p1.pos - p2.pos;
    double e = diff.mag() - stablePoint * stableScale;
    double springf = -springConst * e; // -ke spring force
    double dampf = diff.norm().dot(p2.vel - p1.vel) * dampFact; // damping force
    p1.f += (springf + dampf) * diff.norm(); // equal and opposite reaction
    p2.f -= (springf + dampf) * diff.norm();
}

void simFrame(Matrix<Point>& points) {
    for (int x = 0; x < points.sizeX; x++) {
        for (int y = 0; y < points.sizeY; y++) {
            Point& p = points(x, y);
            if (x < points.sizeX - 1) {  
                if (y < points.sizeY - 1) { 
                    springHandler(p, points(x + 1, y + 1), M_SQRT2); // down right
                }
                springHandler(p, points(x + 1, y), 1.0); // right
            }
            if (y < points.sizeY - 1) {
                if (x > 0) {
                    springHandler(p, points(x - 1, y + 1), M_SQRT2); // down left
                }
                springHandler(p, points(x, y + 1), 1.0); // down
            }
        }
    }
}

void displayFps(double Vfps, double Sfps, sf::RenderWindow& window, sf::Font font){
    sf::Text text;
    text.setFont(font); // font is a sf::Font
    text.setString(std::to_string(Vfps) + " " + std::to_string(Sfps));
    text.setCharacterSize(24); // in pixels, not points!
    text.setFillColor(sf::Color::Red);
    window.draw(text);
}

int main() {
    constexpr Vec2 scale(3, 3);
    constexpr Vec2 gap(0.2, 0.2);
    constexpr Vec2I size(static_cast<int>(scale.x / gap.x), static_cast<int>(scale.y / gap.y));
    constexpr Vec2 simPos(3, 3);
    constexpr double radius = 0.05;
    constexpr double gravity = 0.4;

    sf::Font font;
    if (!font.loadFromFile("arial.ttf")) {
        // error...
    }

    std::cout << Vec2(0,1) - Vec2(0,2) << '\n'; // [0, 1]
    std::cout << (Vec2(0,0) - Vec2(1,1)).mag() << '\n'; // 1.41421
    std::cout << Vec2(1,1).dot(Vec2(2,2)) << '\n'; // 4
    std::cout << Vec2(1,1).dot(Vec2(2,-2)) << '\n'; // 0
    std::cout << Vec2(2,2).norm() << '\n'; // 0

    Point p1(Vec2(3, 3), 1.0, radius);
    Point p2(Vec2(3, 3.2), 1.0, radius);
    springHandler(p1, p2, 1);
    std::cout << p1.f;

    // return (EXIT_SUCCESS);

    // mass is 1 kg per point
    // 1.. is one meter
 
    sf::RenderWindow window(sf::VideoMode(1920, 1080), "Soft Body Simulation"); //, sf::Style::Fullscreen);

    Matrix<Point> points(size.x, size.y);
    for (int x = 0; x < size.x; x++) {
        for (int y = 0; y < size.y; y++) {
            points(x, y) = Point(Vec2(x * gap.x, y * gap.y) + simPos, 1.0, radius);
        } 
    }

    // Point p(Vec2(0, 0), radius, 1.0);

    std::chrono::_V2::system_clock::time_point last = std::chrono::high_resolution_clock::now();
    double Vfps = 0;

    while (window.isOpen()) {
        std::chrono::_V2::system_clock::time_point start = std::chrono::high_resolution_clock::now();

        // clear poll events for sfml
        sf::Event event;
        while (window.pollEvent(event)) {
             if (event.type == sf::Event::Closed) window.close();
        }
        
        std::chrono::nanoseconds sinceVFrame = std::chrono::high_resolution_clock::now() - start;
        while ((sinceVFrame.count() < 10'000'000)) { // TODO: min max avg frames test
            std::chrono::_V2::system_clock::time_point newLast = std::chrono::high_resolution_clock::now();
            std::chrono::nanoseconds deltaTime = newLast - last;
            // std::cout << sinceVFrame.count() << '\n';
            simFrame(points);
            points.forEach([deltaTime] (Point& p) {
                p.update(static_cast<double>(deltaTime.count()) / 1e9, gravity);
                p.polyColHandler();
            });
            last = newLast;
            sinceVFrame = std::chrono::high_resolution_clock::now() - start;
        }

        // draw
        std::chrono::nanoseconds deltaTime = std::chrono::high_resolution_clock::now() - last;  // TODO: avg
        window.clear();       
        double Sfps = 1e9 / static_cast<double>(deltaTime.count());
        displayFps(Vfps, Sfps, window, font);

        points.forEach([&window] (Point& p) {
            p.draw(window);
        });

        window.display();
      
        sinceVFrame = std::chrono::high_resolution_clock::now() - start;
        std::cout << sinceVFrame.count() << "ns\n";
        Vfps = 1e9 / static_cast<double>(sinceVFrame.count());
    }

    return 0;
}