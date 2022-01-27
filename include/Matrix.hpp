#pragma once

#include <array>

template <typename T>
struct Matrix {
    std::vector<T> v;
    int sizeX;
    int sizeY;

    Matrix(int x_, int y_) : v(x_ * y_, T{}), sizeX(x_), sizeY(y_) {}

    T& operator()(int x_, int y_)  {
        return v[x_ + y_ * sizeX];
    }

    const T& operator()(int x_, int y_) const {
        return v[x_ + y_ * sizeX];
    }
};

