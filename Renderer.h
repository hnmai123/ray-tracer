//
// Created by Richard Skarbez on 5/8/24.
//

#ifndef RAYTRACER_RENDERER_H
#define RAYTRACER_RENDERER_H

#include <fstream>

#include "Camera.h"
#include "Color3.h"
#include "Scene.h"

class RendererParameters {
public:
    static RendererParameters defaultParameters() { }
private:
    int imageWidth_ {512};
    int imageHeight_ {512};
    int samplesPerPixel_ {10};
    int maximumRecursionDepth_ {25};
    Color3 backgroundColor_ {0.0, 0.0, 0.0};
    std::string fileName_ {"image.ppm"};
};

class Renderer {
public:
private:
    RendererParameters rendererParams_ {};
    Camera camera_;
    Scene world_;
    std::ofstream outFile_;
};

#endif //RAYTRACER_RENDERER_H
