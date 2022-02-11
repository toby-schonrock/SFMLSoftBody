#define _USE_MATH_DEFINES
#include <cmath>
#include <string>
#include <chrono>
#include <iostream>

#include <SFML/Graphics.hpp>
#include <Vector2.hpp>
#include <Matrix.hpp>
#include <Polygon.hpp>
#include <Point.hpp>

double vsScale = 125.0;

class SoftBody {
private:
    Vec2 scale;
    Vec2 gap;
    Vec2I size;
    Vec2 simPos;
    Matrix<Point> points;
    const double radius = 0.05;
public:
    SoftBody(const Vec2& scale_, const Vec2& gap_, const Vec2& simPos_) : scale(scale_), gap(gap_), size(static_cast<int>(scale.x / gap.x), static_cast<int>(scale.y / gap.y)), simPos(simPos_), points(size.x, size.y) {
        for (int x = 0; x < size.x; x++) {
            for (int y = 0; y < size.y; y++) {
                points(x, y) = Point(Vec2(x * gap.x, y * gap.y) + simPos, 1.0, radius);
            } 
        }
    }

    void draw(sf::RenderWindow& window) {
        for (Point& point: points.v) point.draw(window);
    }

    void simFrame(double deltaTime, double gravity, const std::vector<Polygon>& polys) {
        for (int x = 0; x < points.sizeX; x++) {
            for (int y = 0; y < points.sizeY; y++) {
                Point& p = points(x, y);
                if (x < points.sizeX - 1) {  
                    if (y < points.sizeY - 1) { 
                        Point::springHandler(p, points(x + 1, y + 1), M_SQRT2); // down right
                    }
                    Point::springHandler(p, points(x + 1, y), 1.0); // right
                }
                if (y < points.sizeY - 1) {
                    if (x > 0) {
                        Point::springHandler(p, points(x - 1, y + 1), M_SQRT2); // down left
                    }
                    Point::springHandler(p, points(x, y + 1), 1.0); // down
                }
            }
        }
        for (Point& point: points.v) {
            point.update(deltaTime, gravity);
        }

        for (const Polygon& poly: polys) {
            for (Point& point: points.v) {
                if (poly.isBounded(point.pos)) point.polyColHandler(poly);
            }
        }
    }

};

sf::Vector2f visualize(const Vec2& v) {
    return sf::Vector2f(static_cast<float>(v.x * vsScale), static_cast<float>(v.y * vsScale));
}

void displayFps(double Vfps, double Sfps, sf::RenderWindow& window, const sf::Font& font){
    sf::Text text;
    text.setFont(font); // font is a sf::Font
    text.setString(std::to_string(static_cast<int>(Vfps)) + " " + std::to_string(static_cast<int>(Sfps)));
    text.setCharacterSize(24); // in pixels, not points!
    text.setFillColor(sf::Color::Red);
    window.draw(text);
}

int main() {
    const Vec2I screen(sf::VideoMode::getDesktopMode().width, sf::VideoMode::getDesktopMode().height); 
    vsScale = 125 * screen.x / 2560;  // window scaling

    constexpr double gravity = 2;

    sf::Font font;
    if (!font.loadFromFile("arial.ttf")) {
        std::cout << "Font file not found";
        return (EXIT_FAILURE);
    }

    // std::cout << Vec2(0,1) - Vec2(0,2) << '\n'; // [0, 1]
    // std::cout << (Vec2(0,0) - Vec2(1,1)).mag() << '\n'; // 1.41421
    // std::cout << Vec2(1,1).dot(Vec2(2,2)) << '\n'; // 4
    // std::cout << Vec2(1,1).dot(Vec2(2,-2)) << '\n'; // 0
    // std::cout << Vec2(2,2).norm() << '\n'; // 0

    // Point p1(Vec2(3, 3), 1.0, radius);
    // Point p2(Vec2(3, 3.2), 1.0, radius);
    // springHandler(p1, p2, 1);
    // std::cout << p1.f << '\n';
 
    sf::ContextSettings settings;
    settings.antialiasingLevel = 8;

    sf::RenderWindow window(sf::VideoMode::getDesktopMode(), "Soft Body Simulation", sf::Style::Fullscreen, settings); //, sf::Style::Default);

    SoftBody sb(Vec2(5, 5), Vec2(0.2, 0.2), Vec2(3, 0));

    std::vector<Polygon> polys;
    polys.push_back(Polygon::Square(Vec2(6, 10), -0.75));
    polys.push_back(Polygon::Square(Vec2(14, 10), 0.75));
    polys.push_back(Polygon::Triangle(Vec2(100, 100)));
    polys.push_back(Polygon::Triangle(Vec2(100, 100)));
    polys.push_back(Polygon::Triangle(Vec2(100, 100)));
    polys.push_back(Polygon::Triangle(Vec2(100, 100)));
    polys.push_back(Polygon::Triangle(Vec2(100, 100)));
    polys.push_back(Polygon::Triangle(Vec2(100, 100)));
    

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

            sb.simFrame(static_cast<double>(deltaTime.count()) / 1e9, gravity, polys);
            sinceVFrame = std::chrono::high_resolution_clock::now() - start;
        }

        // draw
        double Sfps = 1e9 * simFrames / static_cast<double>(sinceVFrame.count());
        // double Sfps = simFrames;

        window.clear(); 
        displayFps(Vfps, Sfps, window, font);

        sb.draw(window);
        for (const Polygon& poly: polys) poly.draw(window);

        window.display();
      
        sinceVFrame = std::chrono::high_resolution_clock::now() - start;
        // std::cout << sinceVFrame.count() << "ns\n";
        Vfps = 1e9 / static_cast<double>(sinceVFrame.count());
    }

    return 0;
}