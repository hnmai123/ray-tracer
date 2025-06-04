#ifndef RAYTRACER_COLOR3_H
#define RAYTRACER_COLOR3_H

#include "HelperFunctions.h"
#include "Vector3.h"

class Color3
{
public:
    Color3() = default;
    Color3(double x, double y, double z) : colorVec_{x, y, z} {};

    int r() const { return int(255.999 * colorVec_.x()); }
    int g() const { return int(255.999 * colorVec_.y()); }
    int b() const { return int(255.999 * colorVec_.z()); }

    Color3 operator*(double scale) const
    {
        return {scale * colorVec_.x(), scale * colorVec_.y(), scale * colorVec_.z()};
    }

    Color3 operator*(const Color3 &other) const
    {
        return {
            (colorVec_.x() * other.colorVec_.x()),
            (colorVec_.y() * other.colorVec_.y()),
            (colorVec_.z() * other.colorVec_.z())};
    }

    friend Color3 operator*(double scale, Color3 &vector)
    {
        return vector * scale;
    }

    Color3 operator+(const Color3 other) const
    {
        return {(colorVec_.x() + other.colorVec_.x()),
                (colorVec_.y() + other.colorVec_.y()),
                (colorVec_.z() + other.colorVec_.z())};
    }

    Color3 operator+=(const Color3 &other)
    {
        colorVec_[0] += other.colorVec_[0];
        colorVec_[1] += other.colorVec_[1];
        colorVec_[2] += other.colorVec_[2];
        return *this;
    }

    Color3 correctedAverage(int samplesPerPixel)
    {
        double scale = 1.0 / samplesPerPixel;
        return {
            sqrt(scale * colorVec_.x()),
            sqrt(scale * colorVec_.y()),
            sqrt(scale * colorVec_.z())};
    }

    Color3 &operator/=(double scale)
    {
        colorVec_[0] /= scale;
        colorVec_[1] /= scale;
        colorVec_[2] /= scale;
        return *this;
    }

private:
    Vector3 colorVec_{0.0, 0.0, 0.0};
};

#endif // RAYTRACER_COLOR3_H
