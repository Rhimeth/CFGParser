#include <iostream>
#include <vector>
#include <algorithm>
#include <memory>
#include <cmath>

class Shape {
public:
    virtual double area() const = 0;
    virtual void printInfo() const {
        std::cout << "Generic Shape\n";
    }
    virtual ~Shape() {
        std::cout << "Destroying Shape\n";
    }
};

class Rectangle : public Shape {
    double width, height;
public:
    Rectangle(double w, double h) : width(w), height(h) {
        std::cout << "Rectangle (" << width << "x" << height << ")\n";
    }
    
    double area() const override {
        return width * height;
    }
    
    void printInfo() const override {
        std::cout << "Rectangle: " << width << "x" << height 
                  << " (Area: " << area() << ")\n";
    }
    
    void scale(double factor) {
        if (factor <= 0) {
            std::cerr << "Invalid scale factor\n";
            return;
        }
        width *= factor;
        height *= factor;
    }
};

class Circle : public Shape {
    double radius;
public:
    explicit Circle(double r) : radius(r) {
        std::cout << "Circle (r=" << radius << ")\n";
    }
    
    double area() const override {
        return M_PI * radius * radius;
    }
    
    void printInfo() const override {
        std::cout << "Circle: r=" << radius 
                  << " (Area: " << area() << ")\n";
    }
};

class CompositeShape : public Shape {
    std::vector<std::unique_ptr<Shape>> shapes;
public:
    void addShape(std::unique_ptr<Shape> shape) {
        shapes.push_back(std::move(shape));
    }
    
    double area() const override {
        double total = 0.0;
        for (const auto& shape : shapes) {
            total += shape->area();
        }
        return total;
    }
    
    void printInfo() const override {
        std::cout << "Composite Shape contains " << shapes.size() 
                  << " elements:\n";
        for (const auto& shape : shapes) {
            shape->printInfo();
        }
    }
};

void analyzeShapes(const std::vector<std::unique_ptr<Shape>>& shapes) {
    if (shapes.empty()) {
        std::cout << "No shapes to analyze\n";
        return;
    }

    std::cout << "\n=== Shape Analysis ===\n";
    for (const auto& shape : shapes) {
        shape->printInfo();
    }

    auto [min, max] = std::minmax_element(shapes.begin(), shapes.end(),
        [](const auto& a, const auto& b) {
            return a->area() < b->area();
        });
    
    std::cout << "\nSmallest area: " << (*min)->area() 
              << "\nLargest area: " << (*max)->area() << "\n";
}

int main() {
    std::vector<std::unique_ptr<Shape>> shapes;
    
    shapes.push_back(std::make_unique<Rectangle>(10, 5));
    shapes.push_back(std::make_unique<Circle>(7));
    shapes.push_back(std::make_unique<Rectangle>(3, 6));
    
    auto composite = std::make_unique<CompositeShape>();
    composite->addShape(std::make_unique<Circle>(4));
    composite->addShape(std::make_unique<Rectangle>(2, 8));
    shapes.push_back(std::move(composite));
    
    // Scale the first rectangle
    dynamic_cast<Rectangle*>(shapes[0].get())->scale(1.5);
    
    analyzeShapes(shapes);
    
    return 0;
}