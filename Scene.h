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
private:
    Point3 hitPoint_;
    Vector3 surfaceNormal_;
    //Material surfaceMaterial_;
    double distanceAlongRay_ {infinity};
    bool frontFace_ {true};
};

class Object {
public:
    virtual std::optional<HitRecord> rayHit(const Ray& ray, Interval rayInterval) const = 0;
};

/*
 * TODO
class Sphere : public Object {
public:
    std::optional<HitRecord> rayHit(const Ray& ray, Interval rayInterval) const override {
        // Implement this function
    }
private:
    Point3 centre_ {0.0, 0.0, -1.0};
    double radius_ {0.5};
    // TODO
    // Material material_;
};
 */

/*
 * TODO
class Plane : public Object {
public:
    std::optional<HitRecord> rayHit(const Ray& ray, Interval rayInterval) const override {
        // Implement this function
    }
private:
    Point3 centre_ {0.0, 0.0, -1.0};
    double radius_ {0.5};
    // TODO
    // Material material_;
};
 */

class Scene : public Object {
public:
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
