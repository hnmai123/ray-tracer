#ifndef BVHNODE_H
#define BVHNODE_H

#include <vector>
#include <algorithm>
#include "Scene.h"

class BVHNode : public Object
{
public:
    BVHNode(std::vector<Object *> &objects, size_t start, size_t end)
    {
        int axis = randomInt(0, 2); // Randomly choose an axis (0, 1, or 2)
        auto comparator = [axis](Object *objectA, Object *objectB)
        {
            return objectA->boundingBox().min()[axis] < objectB->boundingBox().min()[axis];
        };

        size_t objectSpan = end - start;
        if (objectSpan == 1)
        {
            left_ = right_ = objects[start];
        }
        else if (objectSpan == 2)
        {
            if (comparator(objects[start], objects[start + 1]))
            {
                left_ = objects[start];
                right_ = objects[start + 1];
            }
            else
            {
                left_ = objects[start + 1];
                right_ = objects[start];
            }
        }
        else
        {
            std::sort(objects.begin() + start, objects.begin() + end, comparator);
            size_t mid = start + objectSpan / 2;
            left_ = new BVHNode(objects, start, mid);
            right_ = new BVHNode(objects, mid, end);
        }
        box_ = surroundingBox(left_->boundingBox(), right_->boundingBox());
    };
    std::optional<HitRecord> rayHit(const Ray &ray, Interval rayInterval) const override
    {
        if (!box_.hit(ray, rayInterval))
            return std::nullopt; // If the ray does not hit the bounding box, return no hit

        auto leftHit = left_->rayHit(ray, rayInterval);
        double rightMax = leftHit ? leftHit->distanceAlongRay() : rayInterval.max(); // If left hit, use its distance, otherwise use the max of the ray interval
        auto rightHit = right_->rayHit(ray, Interval(rayInterval.min(), rightMax));
        if (rightHit)
            return rightHit; // If the right hit is valid, return it
        return leftHit;      // Otherwise, return the left hit (if it exists)
    };

    AABB boundingBox() const override
    {
        return box_;
    };

private:
    Object *left_;
    Object *right_;
    AABB box_;
};

#endif // BVHNODE_H