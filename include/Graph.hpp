#pragma once

#include <deque>
#include <string>
#include <numeric>

class Graph {
    private:
    std::vector<float> v;

    public:
    std::deque<float> data;
    std::pair<std::string, std::string> axis;
    std::size_t buffer;

    Graph(std::pair<std::string, std::string> axis_, const std::size_t& buffer_) : axis(std::move(axis_)), buffer(buffer_) {
        v.reserve(buffer);
    }

    void add(float point) {
        if (data.size() == buffer) data.pop_front(); // if full remove first item
        data.push_back(point);
    }

    float avg() const {
        if (data.empty()) return 0.0f;
        return reduce(data.begin(), data.end()) / static_cast<float>(data.size());
    }

    float* arr(){
        v = {data.begin(), data.end()};
        return &v[0];
    }
};