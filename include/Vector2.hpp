#pragma once

#include <cmath>
#include <ostream>

template <typename T>
class Vector2 {
public:
    T x{};
    T y{};

    constexpr Vector2(T x_, T y_) : x(x_), y(y_) {}

    constexpr Vector2() = default;

    T mag() {
        return std::hypot(x, y);
    }
    Vector2 norm(){
        return *this / this->mag();
    }
    double dot(const Vector2& rhs) const {
        return x * rhs.x + y * rhs.y;
    }

    Vector2& operator += (const Vector2& obj) {
        x += obj.x;
        y += obj.y;
        return *this;
    }
     Vector2& operator -= (const Vector2& obj) {
        x -= obj.x;
        y -= obj.y;
        return *this;
    } 
    Vector2& operator *= (double scale) {
        x *= scale;
        y *= scale;
        return *this;
    }
    Vector2& operator /= (double scale) {
        x /= scale;
        y /= scale;
        return *this;
    }
    friend Vector2 operator + (Vector2 lhs, const Vector2& rhs) {
        return lhs += rhs;
    }
    friend Vector2 operator - (Vector2 lhs, const Vector2& rhs) {
        return lhs -= rhs;
    }
    friend Vector2 operator * (Vector2 lhs, double scale) {
        return lhs *= scale;
    }
    friend Vector2 operator * (double scale, Vector2 rhs) {
        return rhs *= scale;
    }
    friend Vector2 operator / (Vector2 lhs, double scale) {
        return lhs /= scale;
    }
    
    friend std::ostream& operator << (std::ostream& os, const Vector2& v) {
        return os << '[' << v.x << ", " << v.y << ']';
    }
};

using Vec2 = Vector2<double>;
using Vec2I = Vector2<int>;
