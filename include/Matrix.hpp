#include <array>

template <typename T>
class Matrix {
private:
    std::vector<T> v;
public:
    int sizeX;
    int sizeY;

    Matrix(int x_, int y_) : v(x_ * y_, T{}), sizeX(x_), sizeY(y_) {}

    T& operator()(int x_, int y_)  {
        return v[x_ + y_ * sizeX];
    }

    const T& operator()(int x_, int y_) const {
        return v[x_ + y_ * sizeX];
    }

    template <typename Callback>
    void forEach(const Callback& cb) {
        for (int x = 0; x < sizeX; x++) {
            for (int y = 0; y < sizeY; y++) {
                T& p = this-> operator()(x, y);
                cb(p);
            }
        }
    }
};

