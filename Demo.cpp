#include <cmath>
#include <chrono>
#include <thread>
#include <iostream>

#include <SFML/Graphics.hpp>
#include <Vector2.h>

class Point {
public:
    Vector2 pos;
    Vector2 vel = Vector2(0, 0); // set to 0,0
    Vector2 f = Vector2();
    sf::CircleShape shape;
    double mass;
    double radius;

    Point(Vector2 pos_, double mass_, double radius_) : pos(pos_), mass(mass_), radius(radius_) {
        shape = sf::CircleShape(radius);
        shape.setFillColor(sf::Color::Green);
        shape.setPosition(sf::Vector2f(pos.x, pos.y));
        shape.setOrigin(sf::Vector2f(radius, radius));
    }

    void update(sf::RenderWindow& window, double gravity) {
            f += Vector2(0, 1) * (gravity * mass); // add gravity to the force
            vel += f / mass; // * Time.deltaTime; // euler integration could be improved
            pos += vel; // * Time.deltaTime;
            shape.setPosition(sf::Vector2f(pos.x, pos.y));
            //window.draw(shape);

            std::cout << pos << '\n';
            f = Vector2();
        }
};

int main() {
    const double radius = 4.0f;
    const double gravity = 0.4f;
 
    sf::RenderWindow window(sf::VideoMode(1920, 1080), "Soft Body Simulation"); //, sf::Style::Fullscreen);
    Point p(Vector2(10.0, 10.0), 1.0, radius);

    while (window.isOpen()) {
        auto start = std::chrono::high_resolution_clock::now();

        // clear poll events for sfml
        sf::Event event;
        while (window.pollEvent(event)) {
             if (event.type == sf::Event::Closed) window.close();
        }

        p.f = Vector2(1,0);
        p.update(window, gravity);
        window.clear();
        window.draw(p.shape);   
        window.display();
        std::chrono::milliseconds millies = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - start);
        // std::cout << "elapsed time: " << nanos.count() << "ns\n";
        if (millies.count() < 1000) {
            std::this_thread::sleep_for(std::chrono::milliseconds(1000 - millies.count()));
        }
    }

    return 0;
}