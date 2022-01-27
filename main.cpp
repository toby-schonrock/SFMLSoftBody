#define _USE_MATH_DEFINES
#include <cmath>
#include <string>
#include <chrono>
#include <iostream>

#include <SFML/Graphics.hpp>
#include <Vector2.hpp>
#include <Matrix.hpp>

constexpr double vsScale = 125.0;

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
    Polygon(std::vector<Vec2> points_) : points(std::move(points_)), pointCount(points.size()) {
        shape.setPointCount(pointCount);
        boundsUp();
        for (std::size_t x = 0; x < points.size(); x++) shape.setPoint(x, visualize(points[x]));
    }

    bool isBounded(Vec2 pos) const {
        return pos.x >= minBounds.x && pos.y >= minBounds.y && pos.x <= maxBounds.x && pos.y <= maxBounds.y;
    }

    void draw(sf::RenderWindow& window) const {
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

class Point {
public:
    sf::CircleShape shape;
    Vec2 pos;
    Vec2 vel{0, 0}; // set to 0,0
    Vec2 f;
    double mass = 1.0;
    double radius;

    Point() {}

    Point(Vec2 pos_, double mass_, double radius_) : pos(pos_), mass(mass_), radius(radius_) {
        shape = sf::CircleShape(static_cast<float>(radius * vsScale));
        shape.setFillColor(sf::Color::Red);
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

    void polyColHandler(const Polygon& p) {
            bool inside = false;

            double closestDist = DistToEdge(p.points[p.pointCount - 1], p.points[0]);
            Vec2 closestPos = ClosestOnLine(p.points[p.pointCount - 1], p.points[0], closestDist); // test distance to side consisting of last and first vertice

            if (RayCast(p.points[p.pointCount - 1], p.points[0])) inside = !inside;

            for (std::size_t x = 0; x < p.pointCount - 1; x++) { // iterate through all other sides
                double dist = DistToEdge(p.points[x], p.points[x + 1]);
                if (RayCast(p.points[x], p.points[x + 1])) inside = !inside;
                if (closestDist > dist) { // if new closest side found
                    closestPos = ClosestOnLine(p.points[x], p.points[x + 1], dist);
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
        bool RayCast(const Vec2& v1, const Vec2& v2) {
            if ((pos.x < std::min(v1.x, v2.x)) | (pos.x > std::max(v1.x, v2.x))) return false; // if point outisde range of line
            double deltaX = std::abs(v2.x - v1.x);
            if (deltaX == 0.0) return false; // if vertices form a verticle line a verticle line cannot intersect
            double deltaY = v2.y - v1.y;
            // Debug.DrawLine(Vector3.zero, new Vector3(tPos.x - 1, Mathf.Abs(v1.x - tPos.x) / deltaX * deltaY + v1.y - 1, 0), Color.green);
            if (std::abs(v1.x - pos.x) / deltaX * deltaY + v1.y > pos.y) return true;
            return false;
        }

        // using the shortest distance to the line finds the closest point on the line too pos
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

void springHandler(Point& p1, Point& p2, double stableScale) {
    static constexpr double stablePoint = 0.2;
    static constexpr double springConst = 5000;
    static constexpr double dampFact = 100;

    Vec2 diff = p1.pos - p2.pos;
    double e = diff.mag() - stablePoint * stableScale;
    double springf = -springConst * e * stableScale; // -ke spring force
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
    text.setString(std::to_string(static_cast<int>(Vfps)) + " " + std::to_string(static_cast<int>(Sfps)));
    text.setCharacterSize(24); // in pixels, not points!
    text.setFillColor(sf::Color::Red);
    window.draw(text);
}

int main() {
    constexpr Vec2 scale(5, 5);
    constexpr Vec2 gap(0.2, 0.2);
    constexpr Vec2I size(static_cast<int>(scale.x / gap.x), static_cast<int>(scale.y / gap.y));
    constexpr Vec2 simPos(2, -2);
    constexpr double radius = 0.05;
    constexpr double gravity = 2;

    sf::Font font;
    if (!font.loadFromFile("arial.ttf")) {
        std::cout << "Font file not found";
        return (EXIT_FAILURE);
    }

    std::cout << Vec2(0,1) - Vec2(0,2) << '\n'; // [0, 1]
    std::cout << (Vec2(0,0) - Vec2(1,1)).mag() << '\n'; // 1.41421
    std::cout << Vec2(1,1).dot(Vec2(2,2)) << '\n'; // 4
    std::cout << Vec2(1,1).dot(Vec2(2,-2)) << '\n'; // 0
    std::cout << Vec2(2,2).norm() << '\n'; // 0

    Point p1(Vec2(3, 3), 1.0, radius);
    Point p2(Vec2(3, 3.2), 1.0, radius);
    springHandler(p1, p2, 1);
    std::cout << p1.f << '\n';
 
    sf::ContextSettings settings;
    settings.antialiasingLevel = 8;

    sf::RenderWindow window(sf::VideoMode(2560, 1440), "Soft Body Simulation", sf::Style::Fullscreen, settings); //, sf::Style::Default);

    Matrix<Point> points(size.x, size.y);
    for (int x = 0; x < size.x; x++) {
        for (int y = 0; y < size.y; y++) {
            points(x, y) = Point(Vec2(x * gap.x, y * gap.y) + simPos, 1.0, radius);
        } 
    }

    std::vector<Polygon> polys;
    polys.push_back(Polygon::Square(Vec2(6, 10), -0.75));
    polys.push_back(Polygon::Square(Vec2(14, 10), 0.75));
    // polys.push_back(Polygon::Triangle(Vec2(4, 7)));

    std::chrono::_V2::system_clock::time_point last = std::chrono::high_resolution_clock::now();
    double Vfps = 0;

    while (window.isOpen()) {
        std::chrono::_V2::system_clock::time_point start = std::chrono::high_resolution_clock::now();

        // clear poll events for sfml
        sf::Event event;
        while (window.pollEvent(event)) {
             if (event.type == sf::Event::Closed) window.close();
        }

        int simFrames = 0;
        
        std::chrono::nanoseconds sinceVFrame = std::chrono::high_resolution_clock::now() - start;
        while ((sinceVFrame.count() < 10'000'000)) { // TODO: min max avg frames test
            ++simFrames;
            std::chrono::_V2::system_clock::time_point newLast = std::chrono::high_resolution_clock::now();
            constexpr std::chrono::nanoseconds maxFrame{1'000'000};
            std::chrono::nanoseconds deltaTime = std::min(newLast - last, maxFrame);
            last = newLast;

            simFrame(points);
            for (Point& point: points.v) {
                point.update(static_cast<double>(deltaTime.count()) / 1e9, gravity);
            }

            for (const Polygon& poly: polys) {
                for (Point& point: points.v) {
                    if (poly.isBounded(point.pos)) point.polyColHandler(poly);
                }
            }
            sinceVFrame = std::chrono::high_resolution_clock::now() - start;
        }

        // draw
        double Sfps = 1e9 * simFrames / static_cast<double>(sinceVFrame.count());
        // double Sfps = simFrames;

        window.clear(); 
        displayFps(Vfps, Sfps, window, font);

        for (Point& point: points.v) point.draw(window);
        for (const Polygon& poly: polys) poly.draw(window);

        window.display();
      
        sinceVFrame = std::chrono::high_resolution_clock::now() - start;
        // std::cout << sinceVFrame.count() << "ns\n";
        Vfps = 1e9 / static_cast<double>(sinceVFrame.count());
    }

    return 0;
}