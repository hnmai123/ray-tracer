#ifndef RAYTRACER_MATERIAL_H
#define RAYTRACER_MATERIAL_H

#include "Color3.h"
#include "Vector3.h"
#include "Ray.h"
#include <optional>
#include "HitRecord.h"

class Material {
public:
    virtual std::optional<Ray> scatter(const Ray &ray, const HitRecord &hitRecord) const = 0;
    virtual Color3 color() const = 0;
    virtual ~Material() = default;
};

// --- PureDiffuse (Lambertian) ---
class PureDiffuse : public Material {
public:
    PureDiffuse(Color3 color) : albedo_(color) {}

    inline std::optional<Ray> scatter(const Ray &ray, const HitRecord &hitRecord) const override {
        Vector3 scatterDirection = hitRecord.surfaceNormal() + Vector3::randomUnitVector();
        if (scatterDirection.nearZero())
            scatterDirection = hitRecord.surfaceNormal();
        return Ray(hitRecord.hitPoint(), scatterDirection);
    }

    inline Color3 color() const override {
        return albedo_;
    }

private:
    Color3 albedo_;
};

// --- Reflective (Metal) ---
class Reflective : public Material {
public:
    Reflective(Color3 color) : albedo_(color) {}

    inline std::optional<Ray> scatter(const Ray &ray, const HitRecord &hitRecord) const override {
        Vector3 reflectedDirection = ray.direction().unitVector().reflectionAboutNormalVector(hitRecord.surfaceNormal());
        return Ray(hitRecord.hitPoint(), reflectedDirection);
    }

    inline Color3 color() const override {
        return albedo_;
    }

private:
    Color3 albedo_;
};

#endif // RAYTRACER_MATERIAL_H
