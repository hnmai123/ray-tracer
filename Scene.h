//
// Created by Richard Skarbez on 5/8/24.
//

#ifndef RAYTRACER_SCENE_H
#define RAYTRACER_SCENE_H

#include "HelperFunctions.h"
#include "Interval.h"
#include "Ray.h"
#include "Vector3.h"

class HitRecord {
public:
    Point3 hitPoint() const { return hitPoint_; }
    Vector3 surfaceNormal() const { return surfaceNormal_; }
    double distanceAlongRay() const { return distanceAlongRay_; }
    bool frontFace() const { return frontFace_; }

    void setHitPoint(const Point3 &p) { hitPoint_ = p; }
    void setSurfaceNormal(const Vector3 &n) { surfaceNormal_ = n; }
    void setDistanceAlongRay(double t) { distanceAlongRay_ = t; }
    void setFrontFace(bool f) { frontFace_ = f; }

private:
    Point3 hitPoint_;
    Vector3 surfaceNormal_;
    double distanceAlongRay_;
    bool frontFace_;
};

class Object {
public:
    virtual std::optional<HitRecord> rayHit(const Ray& ray, Interval rayInterval) const = 0;
};

class Sphere : public Object
{
public:
    std::optional<HitRecord> rayHit(const Ray &ray, Interval rayInterval) const override
    {
        Vector3 rayToCenter = ray.origin() - centre_;                                     // vector from ray origin to sphere center
        double sqr_ray = ray.direction().length_squared();                                // squared length of ray direction
        double dot_rayCenter_ray = rayToCenter.dot(ray.direction());                      // half of the dot product of rayToCenter and ray direction
        double sqr_rayCenter_radius = rayToCenter.length_squared() - (radius_ * radius_); // squared distance from ray origin to sphere center minus radius squared

        // Ax^2 + Bx + C = 0
        // where A = sqr_ray, B = 2 * dot_rayCenter_ray, C = sqr_rayCenter_radius
        // delta' = B'^2 - AC
        double discriminant = (dot_rayCenter_ray * dot_rayCenter_ray) - (sqr_ray * sqr_rayCenter_radius);
        if (discriminant < 0)
            return std::nullopt; // no intersection

        double sqrt_discriminant = sqrt(discriminant);

        // Calculate the two potential hit distances along the ray
        double hitDistance = (-dot_rayCenter_ray - sqrt_discriminant) / sqr_ray;
        if (!rayInterval.surrounds(hitDistance))
        {
            hitDistance = (-dot_rayCenter_ray + sqrt_discriminant) / sqr_ray;
            if (!rayInterval.surrounds(hitDistance))
            {
                return std::nullopt; // no intersection within the ray interval
            }
        }

        Point3 hitPoint = ray.pointAlongRay(hitDistance);
        Vector3 normalAtHit = (hitPoint - centre_) / radius_; // normalized vector from sphere center to hit point

        HitRecord rec;
        rec.setHitPoint(hitPoint);
        rec.setSurfaceNormal(normalAtHit);
        rec.setDistanceAlongRay(hitDistance);
        rec.setFrontFace(ray.direction().dot(normalAtHit) < 0); // front face if ray direction and normal are in opposite directions
        return rec;
    }

private:
    Point3 centre_{0.0, 0.0, -1.0};
    double radius_{0.5};
    // const Material *material_;
};

class Plane : public Object
{
public:
    std::optional<HitRecord> rayHit(const Ray &ray, Interval rayInterval) const override
    {
        // Assume the plane normal is (0, 1, 0) (y-up), and centre_ is a point on the plane
        Vector3 planeNormal_{0.0, 1.0, 0.0}; // Plane normal
        double demoniator = planeNormal_.dot(ray.direction());

        if (fabs(demoniator) < 1e-8)
            return std::nullopt; // Ray is parallel to the plane
        double distanceToPlane = planeNormal_.dot(centre_ - ray.origin()) / demoniator;

        if (!rayInterval.surrounds(distanceToPlane))
            return std::nullopt; // No intersection within the ray interval
        Point3 hitPoint = ray.pointAlongRay(distanceToPlane);

        HitRecord rec;
        rec.setHitPoint(hitPoint);
        rec.setSurfaceNormal(planeNormal_);
        rec.setDistanceAlongRay(distanceToPlane);
        rec.setFrontFace(ray.direction().dot(planeNormal_) < 0); // front face if ray direction and normal are in opposite directions
        return rec;
    }

private:
    Point3 centre_{0.0, -0.5, 0.0}; // A point on the plane
    // const Material *material_;
};

class Scene : public Object {
public:
    Scene() = default;
    
    Scene(Object* o) {
        add(o);
    }

    void add(Object* o) {
        objects_.push_back(o);
    }

    void clear() {
        objects_.clear();
    }

    std::optional<HitRecord> rayHit(const Ray& ray, Interval rayInterval) const override {
        HitRecord tempHitRecord;
        bool hitAnything = false;
        double closestSoFar = rayInterval.max();

        for (const auto& o : objects_) {
            if (auto tempHit = o->rayHit(ray, Interval(rayInterval.min(), closestSoFar))) {
                hitAnything = true;
                closestSoFar = tempHit->distanceAlongRay();
                tempHitRecord = *tempHit;
            }
        }

        return hitAnything ? std::optional<HitRecord>{tempHitRecord} : std::nullopt;
    }
private:
    std::vector<Object*> objects_{};
};

#endif //RAYTRACER_SCENE_H
