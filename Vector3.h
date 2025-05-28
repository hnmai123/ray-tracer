//
// Created by Richard Skarbez on 5/7/24.
//

#ifndef RAYTRACER_VECTOR3_H
#define RAYTRACER_VECTOR3_H

#include "HelperFunctions.h"


class Vector3 {
public:
    Vector3() = default;
    Vector3(double inX, double inY, double inZ) : x_{inX}, y_{inY}, z_{inZ} {};

    double x() const { return x_; }
    double y() const { return y_; }
    double z() const { return z_; }

    Vector3 operator-() const { return {*this}; }

    Vector3& operator+=(const Vector3& inVec) {
        x_ += inVec.x();
        y_ += inVec.y();
        z_ += inVec.z();

        return *this;
    }

    Vector3& operator*=(double scale) {
        x_ *= scale;
        y_ *= scale;
        z_ *= scale;

        return *this;
    }

    Vector3& operator/=(double scale) {
        return *this *= (1 / scale);
    }

    Vector3 operator+(const Vector3 other) const {
        return {x() + other.x(), y() + other.y(), z() + other.z()};
    }

    Vector3 operator-(const Vector3 other) const {
        return {x() - other.x(), y() - other.y(), z() - other.z()};
    }

    Vector3 operator*(double scale) const {
        return {scale * x(), scale * y(), scale * z()};
    }

    friend Vector3 operator*(double scale, Vector3& vector) {
        return vector * scale;
    }

    Vector3 operator/(double scale) const {
        return *this * (1 / scale);
    }

    friend Vector3 operator/(double scale, Vector3& vector) {
        return vector * (1 / scale);
    }

    double dot(const Vector3& other) const {
        return (x() * other.x()) + (y() * other.y()) + (z() * other.z());
    }

    Vector3 cross(const Vector3 other) const {
        return { (y() * other.z()) - (z() * other.y()),
                 (z() * other.x()) - (x() * other.z()),
                 (x() * other.y()) - (y() * other.x()) };
    }

    friend std::ostream& operator<<(std::ostream& outStream, const Vector3& v) {
        return outStream << v.x() << ' ' << v.y() << ' ' << v.z() ;
    }

    double length_squared() const {
        return x()*x() + y()*y() + z()*z();
    }

    double length() const {
        return sqrt(length_squared());
    }

    Vector3 unitVector() const {
        return {x() / length(), y() / length(), z() / length()};
    }

    bool nearZero(double epsilon = 1e-8) const {
        return (fabs(x()) < epsilon) && (fabs(y()) < epsilon) && (fabs(z()) < epsilon);
    }

    Vector3 reflectionAboutNormalVector(const Vector3& normalVector) const {
        auto self = *this;
        return self - (normalVector * (2 * self.dot(normalVector)));
    }

    Vector3 refractionAboutNormalVector(const Vector3& normalVector, double refractiveIndexRatio) const {
        auto self = *this;
        double cosineTheta = fmin(-self.dot(normalVector), 1.0);
        Vector3 perpendicularComponent = (self + (normalVector * cosineTheta)) * refractiveIndexRatio;
        Vector3 parallelComponent = normalVector * -sqrt(fabs(1.0 - perpendicularComponent.length_squared()));
        return perpendicularComponent + parallelComponent;
    }

    static Vector3 random0to1() {
        return {randomDouble0to1(), randomDouble0to1(), randomDouble0to1()};
    }

    static Vector3 randomInRange(double minimum, double maximum) {
        return {randomDouble(minimum, maximum), randomDouble(minimum, maximum), randomDouble(minimum, maximum)};
    }

    static Vector3 randomInUnitSphere() {
        while (true) {
            Vector3 temp = randomInRange(-1, 1);
            if (temp.length_squared() < 1)
                return temp;
        }
    }

    static Vector3 randomInUnitDisk() {
        while (true) {
            auto temp = Vector3(randomDouble(-1, 1), randomDouble(-1, 1), 0);
            if (temp.length_squared() < 1)
                return temp;
        }
    }

    static Vector3 randomUnitVector() {
        return randomInUnitSphere().unitVector();
    }

    static Vector3 randomOnHemisphere(const Vector3& normalVector) {
        Vector3 temp = randomUnitVector();
        return (temp.dot(normalVector) > 0.0) ? temp : -temp;
    }
private:
    double x_ {0.0};
    double y_ {0.0};
    double z_ {0.0};
};

using Point3 = Vector3;


#endif //RAYTRACER_VECTOR3_H
