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

class Polygon {
private:
    sf::ConvexShape shape;
public:
    Vec2 points[4];
    int pointCount = 4;
    Polygon(Vec2 pos, double tilt) {
        shape.setPointCount(4);
        points[0] = Vec2(2, 0.5) + pos;
        points[1] = Vec2(-2, 0.5) + pos;
        points[2] = Vec2(-2, -0.5 + tilt) + pos;
        points[3] = Vec2(2, -0.5 - tilt) + pos;
        for (int x = 0; x < 4; x++) {
            shape.setPoint(x, visualize(points[x]));
        }
    }

    void draw(sf::RenderWindow& window) {
        window.draw(shape);
    }
};

class Point {
public:
    Vec2 pos;
    sf::CircleShape shape;
    double radius;

    Point() {}

    Point(double radius_) : radius(radius_) {
        shape = sf::CircleShape(static_cast<float>(radius * vsScale));
        shape.setFillColor(sf::Color::Red);
        shape.setPosition(visualize(pos));
        shape.setOrigin(visualize(Vec2(radius, radius)));
    }

    void draw(sf::RenderWindow& window) {
        shape.setPosition(visualize(pos));
        window.draw(shape);
        // std::cout << pos;
    }

    void polyColHandler(Polygon& p) {
            bool inside = false;

            // double closestDist = DistToEdge(p.points[p.pointCount - 1], p.points[0]);
            double closestDist = DistToEdge(p.points[p.pointCount - 1], p.points[0]);
            Vec2 closestPos = ClosestOnLine(p.points[p.pointCount - 1], p.points[0], closestDist); // test distance to side consisting of last and first vertice

            // std::cout << closestDist << '\n';

            // return;

            if (RayCast(p.points[p.pointCount - 1], p.points[0])) inside = !inside;

            for (int x = 0; x < p.pointCount - 1; x++) { // iterate through all other sides
                double dist = DistToEdge(p.points[x], p.points[x + 1]);
                if (RayCast(p.points[x], p.points[x + 1])) inside = !inside;
                if (closestDist > dist) { // if new closest side found
                    closestPos = ClosestOnLine(p.points[x], p.points[x + 1], dist);
                    closestDist = dist;
                }
            }

            std::cout << inside << '\n';

            if (inside) pos = closestPos;
        }

        // cast a verticle ray from infinty to tPos and sees if it collides with the line created between v1 and v2
        bool RayCast(const Vec2& v1, const Vec2& v2) {
            if ((pos.x < std::min(v1.x, v2.x)) | (pos.x > std::max(v1.x, v2.x))) return false; // if point outisde range of line
            double deltaX = std::abs(v2.x - v1.x);
            if (deltaX == 0.0) return false; // if vertices form a verticle line a verticle line cannot intersect
            double deltaY = v2.y - v1.y;
            // Debug.DrawLine(Vector3.zero, new Vector3(tPos.x - 1, Mathf.Abs(v1.x - tPos.x) / deltaX * deltaY + v1.y - 1, 0), Color.green);
            if (std::abs(v1.x - pos.x) / deltaX * deltaY + v1.y > pos.y) return true;
            return false;
        }

        // using the shortest distance to the line finds the closest point on the line too tPos
        Vec2 ClosestOnLine(const Vec2& v1, const Vec2& v2, double dist) { 
            double c2pd = (v1 - pos).mag(); // corner to point distance
            Vec2 result = std::sqrt(c2pd * c2pd - dist * dist) * (v2 - v1).norm(); // pythag
            return result + v1;
        }

        // finds the shortest distance from point to line
        double DistToEdge(const Vec2& v1, const Vec2& v2) { // finds the shortest distance from the point to the edge
            // https://en.wikipedia.org/wiki/Distance_from_a_point_to_a_line#Line_defined_by_two_points
            // draws a traingle between the three points and performs h = 2A/b
            double TArea = std::abs((v2.x - v1.x) * (v1.y - pos.y) - (v1.x - pos.x) * (v2.y - v1.y));
            double TBase = (v1 - v2).mag();
            return TArea / TBase;
        }
};

sf::Vector2f visualize(const Vec2& v) {
    return sf::Vector2f(static_cast<float>(v.x * vsScale), static_cast<float>(v.y * vsScale));
}

int main() {
    // constexpr Vec2 simPos(3, 3);
    constexpr double radius = 0.05;

    sf::Font font;
    if (!font.loadFromFile("arial.ttf")) {
        std::cout << "Font file not found";
        return (EXIT_FAILURE);
    }

    sf::ContextSettings settings;
    settings.antialiasingLevel = 8;

    sf::RenderWindow window(sf::VideoMode(1920, 1080), "Soft Body Simulation", sf::Style::Default, settings); //, sf::Style::Fullscreen);

    Polygon t(Vec2(4,8), -0.5);
    Point p(radius);

    while (window.isOpen()) {
        // clear poll events for sfml
        sf::Event event;
        while (window.pollEvent(event)) {
             if (event.type == sf::Event::Closed) window.close();
        }

        // track point to mouse
        auto m = sf::Mouse::getPosition(window);
        Vec2 mouse(m.x / vsScale, m.y / vsScale);
        p.pos = mouse;

        p.polyColHandler(t);

        // draw
        window.clear();
        t.draw(window);
        p.draw(window);  
        window.display();
      
        }
    return 0;
}