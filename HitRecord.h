// HitRecord.h
#ifndef RAYTRACER_HITRECORD_H
#define RAYTRACER_HITRECORD_H

#include "Vector3.h"
#include "Ray.h"

class Material; // Forward declaration

class HitRecord
{
public:
    Point3 hitPoint() const { return hitPoint_; }
    Vector3 surfaceNormal() const { return surfaceNormal_; }
    double distanceAlongRay() const { return distanceAlongRay_; }
    bool frontFace() const { return frontFace_; }
    const Material *surfaceMaterial() const { return surfaceMaterial_; }

    void setHitPoint(const Point3 &p) { hitPoint_ = p; }
    void setSurfaceNormal(const Vector3 &n) { surfaceNormal_ = n; }
    void setDistanceAlongRay(double t) { distanceAlongRay_ = t; }
    void setFrontFace(const Vector3 &rayDirection, const Vector3 &outwardNormal)
    {
        frontFace_ = rayDirection.dot(outwardNormal) < 0;
        surfaceNormal_ = frontFace_ ? outwardNormal : -outwardNormal;
    }
    void setSurfaceMaterial(const Material *m) { surfaceMaterial_ = m; }

private:
    Point3 hitPoint_;
    Vector3 surfaceNormal_;
    double distanceAlongRay_;
    bool frontFace_;
    const Material *surfaceMaterial_ = nullptr;
};

#endif // RAYTRACER_HITRECORD_H
