#include <cmath>
#include <string>
#include <chrono>
#include <iostream>
#include <numbers>

#include "imgui.h"
#include "imgui-SFML.h"
#include "SFML/Graphics.hpp"
#include "Vector2.hpp"
#include "Matrix.hpp"
#include "Polygon.hpp"
#include "Point.hpp"

float vsScale = 0;

class SoftBody {
public:
    Vec2I size;
    Vec2 simPos;
    float springConst = 8000;
    float dampFact = 100;
    float gap;
private:
    Matrix<Point> points;
    static constexpr float radius = 0.05F;
public:
    SoftBody(const Vec2I& size_, float gap_, const Vec2& simPos_, float springConst_, float dampFact_) :
        size(size_), simPos(simPos_), springConst(springConst_), dampFact(dampFact_), gap(gap_), points(size.x, size.y) {
        for (int x = 0; x < size.x; x++) {
            for (int y = 0; y < size.y; y++) {
                points(x, y) = Point(Vec2(x, y) * gap + simPos, 1.0, radius);
            }
        }
    }

    void reset() { // evil function 
        *this = SoftBody(size, gap, simPos, springConst, dampFact);
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
                        Point::springHandler(p, points(x + 1, y + 1), std::numbers::sqrt2 * gap, springConst, dampFact); // down right
                    }
                    Point::springHandler(p, points(x + 1, y), gap, springConst, dampFact); // right
                }
                if (y < points.sizeY - 1) {
                    if (x > 0) {
                        Point::springHandler(p, points(x - 1, y + 1), std::numbers::sqrt2 * gap, springConst, dampFact); // down left
                    }
                    Point::springHandler(p, points(x, y + 1), gap, springConst, dampFact); // down
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
    return sf::Vector2f(static_cast<float>(v.x), static_cast<float>(v.y)) * vsScale;
}

void displayFps(double Vfps, double Sfps, sf::RenderWindow& window, const sf::Font& font){
    sf::Text text;
    text.setFont(font); // font is a sf::Font
    text.setString(std::to_string(static_cast<int>(Vfps)) + " " + std::to_string(static_cast<int>(Sfps)));
    text.setCharacterSize(24); // in pixels, not points!
    text.setFillColor(sf::Color::Red);
    window.draw(text);
}

void displayImGui(SoftBody& sb, float& gravity) {
    ImGui::Begin("Settings");
    ImGui::DragFloat("Gravity", &gravity, 0.01f);
    ImGui::DragFloat("Gap", &sb.gap, 0.005f);
    ImGui::DragFloat("Spring Constant", &sb.springConst, 10.0f, 0.0F, 20000.0F);
    ImGui::DragFloat("Damping Factor", &sb.dampFact, 1.0f, 0.0F, 300.0F);
    ImGui::DragInt("Size X", &sb.size.x, 1, 2, 50);
    ImGui::DragInt("Size Y", &sb.size.y, 1, 2, 50);
    ImGui::DragFloat("Zoom", &vsScale, 1, 0, 250);
    if (ImGui::Button("Reset sim")) sb.reset();
    ImGui::SameLine();
    if (ImGui::Button("Default sim")) {
        sb = SoftBody(Vec2I(25, 25), 0.2F, Vec2(3, 0), 8000, 100);
        gravity = 2.0F;
    }
}

int main() {
    float gravity = 2;

    const Vec2I screen(sf::VideoMode::getDesktopMode().width, sf::VideoMode::getDesktopMode().height);
    vsScale = 25.0F/512.0F * static_cast<float>(screen.x);  // window scaling

    sf::Font font;
    if (!font.loadFromFile("arial.ttf")) {
        std::cout << "Font file not found";
        return (EXIT_FAILURE);
    }

    sf::ContextSettings settings;
    settings.antialiasingLevel = 8;

    sf::RenderWindow window(sf::VideoMode::getDesktopMode(), "Soft Body Simulation", sf::Style::Fullscreen, settings); //, sf::Style::Default);
    ImGui::SFML::Init(window);

    SoftBody sb(Vec2I(25, 25), 0.2F, Vec2(3, 0), 8000, 100);

    std::vector<Polygon> polys;
    polys.push_back(Polygon::Square(Vec2(6, 10), -0.75));
    polys.push_back(Polygon::Square(Vec2(14, 10), 0.75));
    polys.push_back(Polygon::Triangle(Vec2(100, 100)));

    std::chrono::_V2::system_clock::time_point last = std::chrono::high_resolution_clock::now();
    double Vfps = 0;

    sf::Clock deltaClock; // for imgui - read https://eliasdaler.github.io/using-imgui-with-sfml-pt1/
    while (window.isOpen()) {
        std::chrono::_V2::system_clock::time_point start = std::chrono::high_resolution_clock::now();

        // clear poll events for sfml and imgui
        sf::Event event;
        while (window.pollEvent(event)) {
            ImGui::SFML::ProcessEvent(event);
            if (event.type == sf::Event::Closed) window.close();
        }

        ImGui::SFML::Update(window, deltaClock.restart());
        displayImGui(sb, gravity);

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
        for (Polygon& poly: polys) poly.draw(window);

        ImGui::End();
        ImGui::SFML::Render(window); // end and draw
        window.display();
      
        sinceVFrame = std::chrono::high_resolution_clock::now() - start;
        // std::cout << sinceVFrame.count() << "ns\n";
        Vfps = 1e9 / static_cast<double>(sinceVFrame.count());
    }
    ImGui::SFML::Shutdown();

    return 0;
}