#ifndef RAYTRACER_AABB_H
#define RAYTRACER_AABB_H

#include "Vector3.h"
#include "Ray.h"
#include "Interval.h"
#include <algorithm>

class AABB
{
public:
    AABB() {}
    AABB(const Point3 &min, const Point3 &max) : min_(min), max_(max) {}
    const Point3 &min() const { return min_; }
    const Point3 &max() const { return max_; }

    bool hit(const Ray &ray, Interval rayInterval) const
    {
        for (int axisIndex = 0; axisIndex < 3; ++axisIndex)
        {
            double inverseDirection = 1.0 / ray.direction()[axisIndex];
            double tNear = (min_[axisIndex] - ray.origin()[axisIndex]) * inverseDirection;
            double tFar = (max_[axisIndex] - ray.origin()[axisIndex]) * inverseDirection;
            if (inverseDirection < 0.0)
                std::swap(tNear, tFar);
            double intervalMin = std::max(tNear, intervalMin);
            double intervalMax = std::min(tFar, intervalMax);
            if (intervalMax <= intervalMin) // No intersection along this axis
                return false;
        }
        return true; // Intersection occurs
    }

private:
    Point3 min_;
    Point3 max_;
};

inline AABB surroundingBox(const AABB &box0, const AABB &box1)
{
    Point3 small(
        std::min(box0.min().x(), box1.min().x()),
        std::min(box0.min().y(), box1.min().y()),
        std::min(box0.min().z(), box1.min().z()));
    Point3 large(
        std::max(box0.max().x(), box1.max().x()),
        std::max(box0.max().y(), box1.max().y()),
        std::max(box0.max().z(), box1.max().z()));
    return AABB(small, large);
}

#endif // RAYTRACER_AABB_H