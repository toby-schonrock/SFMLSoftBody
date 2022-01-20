#include <cmath>
#include <ostream>

class Vector2Int {
public:
    int x;
    int y;

    Vector2Int(int x_, int y_) : x(x_), y(y_) {}

    Vector2Int() : x{}, y{} {}

    int mag() {
        return std::hypot(x, y);
    }
    Vector2Int norm(){
        return *this / this->mag();
    }

    Vector2Int& operator += (const Vector2Int& obj) {
        x += obj.x;
        y += obj.y;
        return *this;
    }
     Vector2Int& operator -= (const Vector2Int& obj) {
        x -= obj.x;
        y -= obj.y;
        return *this;
    } 
    Vector2Int& operator *= (const int& scale) {
        x *= scale;
        y *= scale;
        return *this;
    }
    Vector2Int& operator /= (const int& scale) {
        x /= scale;
        y /= scale;
        return *this;
    }
    friend Vector2Int operator + (Vector2Int lhs, const Vector2Int& rhs) {
        return lhs += rhs;
    }
    friend Vector2Int operator - (Vector2Int lhs, const Vector2Int& rhs) {
        return lhs -= rhs;
    }
    friend Vector2Int operator * (Vector2Int lhs, const int& scale) {
        return lhs *= scale;
    }
    friend Vector2Int operator * (const int& scale, Vector2Int rhs) {
        return rhs *= scale;
    }
    friend Vector2Int operator / (Vector2Int lhs, const int& scale) {
        return lhs /= scale;
    }
    
    friend std::ostream& operator << (std::ostream& os, const Vector2Int& v) {
        return os << '[' << v.x << ", " << v.y << ']';
    }
};