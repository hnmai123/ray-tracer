#ifndef RAYTRACER_RAY_H
#define RAYTRACER_RAY_H

#include "Vector3.h"

class Ray {
public:
    Ray(const Point3& o, const Vector3& d) : origin_{o}, direction_{d} {};
    Point3 origin() const { return origin_; }
    Vector3 direction() const { return direction_; }
    Vector3 pointAlongRay(double distance) const { return origin() + (direction() * distance); }
private:
    Point3 origin_;
    Vector3 direction_;
};

#endif //RAYTRACER_RAY_H
