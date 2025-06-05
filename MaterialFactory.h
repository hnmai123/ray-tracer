#ifndef RAYTRACER_MATERIAL_FACTORY_H
#define RAYTRACER_MATERIAL_FACTORY_H

#include "Material.h"

class MaterialFactory
{
public:
    static const Material *createReflective(const Color3 &color)
    {
        return new Reflective(color);
    }

    static const Material *createGlossy(const Color3 &color, double glossiness)
    {
        return new Glossy(color, glossiness);
    }

    static const Material *createDiffuse(const Color3 &color)
    {
        return new PureDiffuse(color);
    }

    static const Material *createEmissive(const Color3 &color)
    {
        return new Emissive(color);
    }

    static const Material *createChecker(const Color3 &color1, const Color3 &color2, double scale)
    {
        return new Checker(color1, color2, scale);
    }

    static const Material *createDielectric(double refractiveIndex)
    {
        return new Dielectric(refractiveIndex);
    }
};

#endif // RAYTRACER_MATERIAL_FACTORY_H