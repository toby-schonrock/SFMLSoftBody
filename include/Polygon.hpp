#pragma once

#include <SFML/Graphics.hpp>
#include <Vector2.hpp>

sf::Vector2f visualize(const Vec2& v);

class Polygon {
private:
    sf::ConvexShape shape;
    void boundsUp () {
        maxBounds = points[0];
        minBounds = points[0];
        for (std::size_t x = 1; x < points.size(); x++) {
            maxBounds.x = std::max(maxBounds.x, points[x].x);
            maxBounds.y = std::max(maxBounds.y, points[x].y);
            minBounds.x = std::min(minBounds.x, points[x].x);
            minBounds.y = std::min(minBounds.y, points[x].y);
        }
    } 
public:
    std::vector<Vec2> points;
    Vec2 maxBounds;
    Vec2 minBounds;
    std::size_t pointCount;
    explicit Polygon(std::vector<Vec2> points_) : points(std::move(points_)), pointCount(points.size()) {
        shape.setPointCount(pointCount);
        boundsUp();
        for (std::size_t x = 0; x < points.size(); x++) shape.setPoint(x, visualize(points[x]));
    }

    bool isBounded(Vec2 pos) const {
        return pos.x >= minBounds.x && pos.y >= minBounds.y && pos.x <= maxBounds.x && pos.y <= maxBounds.y;
    }

    void draw(sf::RenderWindow& window) {
        for (std::size_t x = 0; x < points.size(); x++) shape.setPoint(x, visualize(points[x]));
        window.draw(shape);
    }

    // static stuff
    static Polygon Square(Vec2 pos, double tilt) {
        return Polygon({Vec2(4, 0.5) + pos, 
                        Vec2(-4, 0.5) + pos, 
                        Vec2(-4, -0.5 + tilt) + pos, 
                        Vec2(4, -0.5 - tilt) + pos});
    }

    static Polygon Triangle(Vec2 pos) { 
        return Polygon({Vec2(1, 1) + pos,
                        Vec2(-1, 1) + pos,
                        Vec2(0, -1) + pos});
    }
};