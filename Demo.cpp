#include <cmath>
#include <chrono>
#include <thread>
#include <iostream>

#include <SFML/Graphics.hpp>
#include <Vector2.h>
#include <Vector2Int.h>

const double r2 = 1.4142857;
const Vector2Int scale(2, 2);
const Vector2 gap(0.2, 0.2);
const Vector2Int size(scale.x / gap.x, scale.y / gap.y);

Vector2 pos(100, 100);

class Point {
public:
    Vector2 pos;
    Vector2 vel = Vector2(0, 0); // set to 0,0
    Vector2 f = Vector2();
    sf::CircleShape shape;
    double mass;
    double radius;

    Point() {}

    Point(Vector2 pos_, double mass_, double radius_) : pos(pos_), mass(mass_), radius(radius_) {
        shape = sf::CircleShape(radius);
        shape.setFillColor(sf::Color::Green);
        shape.setPosition(sf::Vector2f(pos.x, pos.y));
        shape.setOrigin(sf::Vector2f(radius, radius));
    }

    void update(double deltaTime, sf::RenderWindow& window, double gravity) {
            f += Vector2(0, 1) * (gravity * mass); // add gravity to the force
            vel += (f * deltaTime) / mass; // euler integration could be improved
            pos += vel * deltaTime;
            shape.setPosition(sf::Vector2f(pos.x, pos.y));
            window.draw(shape);
            // std::cout << deltaTime << '\n';

            // std::cout << pos << '\n';
            f = Vector2();
        }
};

void springHandler(Point& p1, Point& p2, double stableScale) {
    Vector2 diff = p1.pos - p2.pos;
}

void simFrame(Point points[]) {
    for (int x = 0; x < size.x; x++) {
            for (int y = 0; y < size.y; y++) {
                if (x < size.x - 1) {  // to the right
                    if (y < size.y - 1) { // to the right and above
                        springHandler(points[x + y * size.y], points[x + 1 + (y + 1) * size.y], r2);
                    }
                    springHandler(points[x + y * size.y], points[x + 1 + y * size.y], 1.0);
                }
                if (y < size.y - 1) { // above
                    if (x > 0) { // to the left and above
                        springHandler(points[x + y * size.y], points[x - 1 + y * size.y], r2);
                    }
                    springHandler(points[x + y * size.y], points[x - 1 + (y + 1) * size.y], 1.0);
                }
            }
        }
}

int main() {
    const double radius = 4.0f;
    const double gravity = 0.4f;
 
    sf::RenderWindow window(sf::VideoMode(1920, 1080), "Soft Body Simulation"); //, sf::Style::Fullscreen);
    Point p(Vector2(10.0, 10.0), 2.0, radius);

    Point points[size.x * size.y];
    for (int x = 0; x < size.x * size.y; x++) points[x] = Point()

    std::chrono::_V2::system_clock::time_point last = std::chrono::high_resolution_clock::now();

    while (window.isOpen()) {
        std::chrono::_V2::system_clock::time_point start = std::chrono::high_resolution_clock::now();

        // clear poll events for sfml
        sf::Event event;
        while (window.pollEvent(event)) {
             if (event.type == sf::Event::Closed) window.close();
        }

        std::chrono::nanoseconds nanos = std::chrono::high_resolution_clock::now() - last;
        last = std::chrono::high_resolution_clock::now();
        window.clear();
        std::cout << nanos.count() << '\n';
        simFrame(points);
        p.update(nanos.count() / 1000000.0, window, gravity);

        window.display();
        nanos = std::chrono::high_resolution_clock::now() - start;
        // std::cout << "elapsed time: " << nanos.count() << "ns\n";
        while ((nanos.count() < 100000)) {
            nanos = std::chrono::high_resolution_clock::now() - start;
            // std::this_thread::sleep_for(std::chrono::nanoseconds(100000 - nanos.count()));
        }
    }

    return 0;
}