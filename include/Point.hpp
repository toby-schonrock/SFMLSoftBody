#pragma once

#include <SFML/Graphics.hpp>
#include <Vector2.hpp>
#include <Polygon.hpp>

extern float vsScale;

class Point {
public:
    sf::CircleShape shape;
    Vec2 pos;
    Vec2 vel{0, 0}; // set to 0,0
    Vec2 f;
    double mass = 1.0;
    float radius;

    Point() {}

    Point(Vec2 pos_, double mass_, float radius_) : pos(pos_), mass(mass_), radius(radius_) {
        shape = sf::CircleShape(radius * vsScale);
        shape.setFillColor(sf::Color::Red);
        shape.setPosition(visualize(pos));
        shape.setOrigin(visualize(Vec2(radius, radius)));
    }

    void draw(sf::RenderWindow& window) {
        shape.setRadius(radius * vsScale);
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

    void polyColHandler(const Polygon& poly) {
        bool inside = false;

        double closestDist = DistToEdge(poly.points[poly.pointCount - 1], poly.points[0]);
        Vec2 closestPos = ClosestOnLine(poly.points[poly.pointCount - 1], poly.points[0], closestDist); // test distance to side consisting of last and first vertice

        if (RayCast(poly.points[poly.pointCount - 1], poly.points[0])) inside = !inside;

        for (std::size_t x = 0; x < poly.pointCount - 1; x++) { // iterate through all other sides
            double dist = DistToEdge(poly.points[x], poly.points[x + 1]);
            if (RayCast(poly.points[x], poly.points[x + 1])) inside = !inside;
            if (closestDist > dist) { // if new closest side found
                closestPos = ClosestOnLine(poly.points[x], poly.points[x + 1], dist);
                closestDist = dist;
            }
        }
        if (inside) {
            if (closestDist > 1e-10){ // to prevent the norm() dividing by ~ 0
                Vec2 normal = (closestPos - pos);
                normal = normal.norm();
                vel -= (2 * normal.dot(vel) * normal);
                pos = closestPos;
            }
        }
    }

    // cast a verticle ray from infinty to tPos and sees if it collides with the line created between v1 and v2
    bool RayCast(const Vec2& v1, const Vec2& v2) const {
        if ((pos.x < std::min(v1.x, v2.x)) | (pos.x > std::max(v1.x, v2.x))) return false; // if point outisde range of line
        double deltaX = std::abs(v2.x - v1.x);
        if (deltaX == 0.0) return false; // if vertices form a verticle line a verticle line cannot intersect
        double deltaY = v2.y - v1.y;
        // Debug.DrawLine(Vector3.zero, new Vector3(tPos.x - 1, Mathf.Abs(v1.x - tPos.x) / deltaX * deltaY + v1.y - 1, 0), Color.green);
        if (std::abs(v1.x - pos.x) / deltaX * deltaY + v1.y > pos.y) return true;
        return false;
    }

    // using the shortest distance to the line finds the closest point on the line too pos
    Vec2 ClosestOnLine(const Vec2& v1, const Vec2& v2, double dist) const { 
        double c2pd = (v1 - pos).mag(); // corner to point distance
        Vec2 result = std::sqrt(c2pd * c2pd - dist * dist) * (v2 - v1).norm(); // pythag
        return result + v1;
    }

    // finds the shortest distance from point to line
    double DistToEdge(const Vec2& v1, const Vec2& v2) const { // finds the shortest distance from the point to the edge
        // https://en.wikipedia.org/wiki/Distance_from_a_point_to_a_line#Line_defined_by_two_points
        // draws a traingle between the three points and performs h = 2A/b
        double TArea = std::abs((v2.x - v1.x) * (v1.y - pos.y) - (v1.x - pos.x) * (v2.y - v1.y));
        double TBase = (v1 - v2).mag();
        return TArea / TBase;
    }

    
    static void springHandler(Point& p1, Point& p2, double stablePoint, float springConst, float dampFact) { 
        Vec2 diff = p1.pos - p2.pos;  // broken out alot "yes this is faster! really like 3x" 
        double diffMag = diff.mag();
        Vec2 diffNorm = diff / diffMag; 
        double ext = diffMag - stablePoint;
        double springf = -springConst * ext; // -ke spring force and also if a diagonal increase spring constant for stability // test
        double dampf = diffNorm.dot(p2.vel - p1.vel) * dampFact; // damping force
        Vec2 force = (springf + dampf) * diffNorm;
        p1.f += force; // equal and opposite reaction
        p2.f -= force;
    }
};