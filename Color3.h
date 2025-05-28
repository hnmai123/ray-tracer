//
// Created by Richard Skarbez on 5/8/24.
//

#ifndef RAYTRACER_COLOR3_H
#define RAYTRACER_COLOR3_H

#include "HelperFunctions.h"
#include "Vector3.h"

class Color3 {
public:
    Color3() = default;
    Color3(double x, double y, double z) : colorVec_{x, y, z} { };

    int r() const { return int(255.999 * colorVec_.x()); }
    int g() const { return int(255.999 * colorVec_.y()); }
    int b() const { return int(255.999 * colorVec_.z()); }

    Color3 operator*(double scale) const {
        return {scale * colorVec_.x(), scale * colorVec_.y(), scale * colorVec_.z()};
    }

    friend Color3 operator*(double scale, Color3& vector) {
        return vector * scale;
    }

    Color3 operator+(const Color3 other) const {
        return {(colorVec_.x() + other.colorVec_.x()),
                (colorVec_.y() + other.colorVec_.y()),
                (colorVec_.z() + other.colorVec_.z())};
    }

private:
    Vector3 colorVec_ {0.0, 0.0, 0.0};
};

#endif //RAYTRACER_COLOR3_H
