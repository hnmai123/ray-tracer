#ifndef RAYTRACER_MATERIAL_H
#define RAYTRACER_MATERIAL_H

#include "Color3.h"
#include "Vector3.h"
#include "Ray.h"
#include <optional>
#include "HitRecord.h"

class Material
{
public:
    virtual std::optional<Ray> scatter(const Ray &ray, const HitRecord &hitRecord) const = 0;
    virtual Color3 color() const = 0;
    virtual ~Material() = default;
    virtual Color3 emittedColor() const
    {
        return Color3(0, 0, 0); // Default implementation returns no emission
    }
};

// --- PureDiffuse (Lambertian) ---
class PureDiffuse : public Material
{
public:
    PureDiffuse(Color3 color) : albedo_(color) {}

    inline std::optional<Ray> scatter(const Ray &ray, const HitRecord &hitRecord) const override
    {
        Vector3 scatterDirection = hitRecord.surfaceNormal() + Vector3::randomUnitVector();
        if (scatterDirection.nearZero())
            scatterDirection = hitRecord.surfaceNormal();
        return Ray(hitRecord.hitPoint(), scatterDirection);
    }

    inline Color3 color() const override
    {
        return albedo_;
    }

private:
    Color3 albedo_;
};

// --- Reflective (Metal) ---
class Reflective : public Material
{
public:
    Reflective(Color3 color) : albedo_(color) {}

    inline std::optional<Ray> scatter(const Ray &ray, const HitRecord &hitRecord) const override
    {
        Vector3 reflectedDirection = ray.direction().unitVector().reflectionAboutNormalVector(hitRecord.surfaceNormal());
        return Ray(hitRecord.hitPoint(), reflectedDirection);
    }

    inline Color3 color() const override
    {
        return albedo_;
    }

private:
    Color3 albedo_;
};

class Glossy : public Material
{
public:
    Glossy(Color3 color, double gloss = 0.0) : albedo_(color), glossiness_(std::clamp(gloss, 0.0, 1.0)) {}
    inline std::optional<Ray> scatter(const Ray &ray, const HitRecord &hitRecord) const override
    {
        Vector3 reflectedDirection = ray.direction().unitVector().reflectionAboutNormalVector(hitRecord.surfaceNormal());

        Vector3 glossyDirection = reflectedDirection + Vector3::randomInUnitSphere() * glossiness_;
        if (glossyDirection.dot(hitRecord.surfaceNormal()) <= 0)
            glossyDirection = hitRecord.surfaceNormal();
        return Ray(hitRecord.hitPoint(), glossyDirection);
    }
    inline Color3 color() const override
    {
        return albedo_;
    }

private:
    Color3 albedo_;
    double glossiness_;
};

// --- Emissive (Light Source) ---
class Emissive : public Material
{
public:
    Emissive(Color3 color) : emittedColor_(color) {}
    inline std::optional<Ray> scatter(const Ray &ray, const HitRecord &hitRecord) const override
    {
        return std::nullopt; // Emissive materials do not scatter rays
    }
    inline Color3 color() const override
    {
        return emittedColor_;
    }
    inline Color3 emittedColor() const override
    {
        return emittedColor_; // Return the emitted color
    }

private:
    Color3 emittedColor_;
};

// --- Checker (Simple Checker) --- (Caro)
class Checker : public Material
{
public:
    Checker(Color3 color1, Color3 color2, double scale = 1.0)
        : color1_(color1), color2_(color2), scale_(scale) {}

    std::optional<Ray> scatter(const Ray &ray, const HitRecord &rec) const override
    {
        Vector3 scatterDir = rec.surfaceNormal() + Vector3::randomUnitVector();
        if (scatterDir.nearZero())
            scatterDir = rec.surfaceNormal();
        return Ray(rec.hitPoint(), scatterDir);
    }

    Color3 color() const override
    {
        return Color3(1, 1, 1); // not used directly
    }

    Color3 emittedColor() const override
    {
        return Color3(0, 0, 0);
    }

    Color3 checkerColorAt(const Point3 &p) const
    {
        // checker based on x and z only (for horizontal plane)
        int check = static_cast<int>(floor(p.x() * scale_)) + static_cast<int>(floor(p.z() * scale_));
        return (check % 2 == 0) ? color1_ : color2_;
    }

private:
    Color3 color1_, color2_;
    double scale_;
};
// -- Dielectric (Transparent) ---
class Dielectric : public Material
{
public:
    Dielectric(double refractiveIndex) : refractiveIndex_(refractiveIndex) {}
    std::optional<Ray> scatter(const Ray &ray, const HitRecord &hitRecord) const override
    {
        double refractionRatio = hitRecord.frontFace() ? (1.0 / refractiveIndex_) : refractiveIndex_;

        Vector3 unitDirection = ray.direction().unitVector();
        double cosTheta = std::min((-unitDirection).dot(hitRecord.surfaceNormal()), 1.0);
        double sinTheta = sqrt(1.0 - cosTheta * cosTheta);

        bool cannotRefract = refractionRatio * sinTheta > 1.0;
        Vector3 scatterDirection;
        if (cannotRefract || reflectance(cosTheta, refractionRatio) > randomDouble0to1())
        {
            // Reflect
            scatterDirection = unitDirection.reflectionAboutNormalVector(hitRecord.surfaceNormal());
        }
        else
        {
            // Refract
            scatterDirection = unitDirection.refractionAboutNormalVector(hitRecord.surfaceNormal(), refractionRatio);
        }
        return Ray(hitRecord.hitPoint(), scatterDirection);
    }

    Color3 color() const override
    {
        return Color3(1, 1, 1); // not used directly
    }

private:
    double refractiveIndex_;
    static double reflectance(double cosineTheta, double refractiveIndexRatio)
    {
        // Schlick's approximation for reflectance
        double r0 = (1 - refractiveIndexRatio) / (1 + refractiveIndexRatio);
        r0 *= r0;
        return r0 + (1 - r0) * pow((1 - cosineTheta), 5);
    }
};
#endif // RAYTRACER_MATERIAL_H
