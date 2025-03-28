#include <iostream>
#include <vector>
#include <map>
#include <memory>
#include <thread>
#include <mutex>
#include <stdexcept>

// Global Variable
std::mutex globalMutex;

// Base Class with Virtual Method
class Shape {
public:
    virtual double area() const = 0;  // Pure virtual function
    virtual ~Shape() {}
};

// Derived Class
class Rectangle : public Shape {
    double width, height;
public:
    Rectangle(double w, double h) : width(w), height(h) {}
    double area() const override { return width * height; }
};

// Function Template
template <typename T>
T maxElement(const std::vector<T>& vec) {
    if (vec.empty()) {
        throw std::runtime_error("Vector is empty");
    }

    T maxVal = vec[0];
    for (const T& val : vec) {
        if (val > maxVal) maxVal = val;
    }
    return maxVal;
}

