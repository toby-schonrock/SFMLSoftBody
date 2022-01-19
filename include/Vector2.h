#include <cmath>
#include <ostream>

class Vector2 {
public:
    double x;
    double y;

    Vector2(double x_, double y_) : x(x_), y(y_) {}

    Vector2() : x{}, y{} {}

    double mag() {
        return std::hypot(x, y);
    }
    Vector2 norm(){
        return *this / this->mag();
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
    Vector2& operator *= (const double& scale) {
        x *= scale;
        y *= scale;
        return *this;
    }
    Vector2& operator /= (const double& scale) {
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
    friend Vector2 operator * (Vector2 lhs, const double& scale) {
        return lhs *= scale;
    }
    friend Vector2 operator * (const double& scale, Vector2 rhs) {
        return rhs *= scale;
    }
    friend Vector2 operator / (Vector2 lhs, const double& scale) {
        return lhs /= scale;
    }
    
    friend std::ostream& operator << (std::ostream& os, const Vector2& v) {
        return os << '[' << v.x << ", " << v.y << ']';
    }
};