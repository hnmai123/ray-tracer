#include <iostream>
#include <fstream>
#include "BVHNode.h"
#include "Renderer.h"
#include "MaterialFactory.h"

int main()
{
    int imageWidth = 512;
    int imageHeight = 512;

    Camera camera = Camera(
        Point3(0, 0, 10),
        Point3(0, 0, -1.5),
        512, 1.0);

    // Create materials of diffuse and reflective types
    const Material *greenDiffuse = MaterialFactory::createDiffuse(Color3(0.3, 0.8, 0.3));                             // green color
    const Material *greenGlossy = MaterialFactory::createGlossy(Color3(0.2, 0.8, 0.2), 0.1);                          // glossy green color
    const Material *redDiffuse = MaterialFactory::createDiffuse(Color3(0.8, 0.2, 0.2));                               // red color
    const Material *pinkMirror = MaterialFactory::createReflective(Color3(1, 0.6, 0.8));                              // pink color
    const Material *goldGlossy = MaterialFactory::createGlossy(Color3(1.0, 0.84, 0.0), 0.5);                          // gold color with slight glossiness
    const Material *sunEmissive = MaterialFactory::createEmissive(Color3(0.9, 0.84, 0.48));                           // sun color (light source)
    const Material *caroChecker = MaterialFactory::createChecker(Color3(0.4, 0.2, 0.1), Color3(0.8, 0.6, 0.3), 10.0); // Caro checker
    const Material *dielectric = MaterialFactory::createDielectric(2.417);                                            // Diamond-like material with high refractive index
    const Material *caroChecker2 = MaterialFactory::createChecker(Color3(0, 0, 0), Color3(1, 1, 1), 10.0);            // Inverted caro checker

    // Create objects list for BVH tree
    std::vector<Object *> objects;

    // Emissive sphere (light source)
    Sphere *lightSphere = new Sphere(Point3(0.0, 0.7, -1.5), 0.1, sunEmissive); // Light source sphere
    objects.push_back(lightSphere);

    // Add spheres with different materials
    objects.push_back(new Sphere(Point3(-0.75, -0.3, -1), 0.2, redDiffuse));
    objects.push_back(new Sphere(Point3(-0.25, -0.3, -1), 0.2, dielectric));
    objects.push_back(new Sphere(Point3(0.25, -0.3, -1), 0.2, goldGlossy));
    objects.push_back(new Sphere(Point3(0.75, -0.3, -1), 0.2, pinkMirror));
    objects.push_back(new Sphere(Point3(-0.75, -0.25, -2), 0.25, greenDiffuse)); // Green sphere
    objects.push_back(new Sphere(Point3(0, -0.25, -2), 0.25, caroChecker2));  // Glossy green sphere
    objects.push_back(new Sphere(Point3(0.75, -0.25, -2), 0.25, greenGlossy));   // Glossy green sphere

    // Ground plane
    objects.push_back(new Plane(Point3(0, -0.5, 0), caroChecker)); // Large ground plane

    // Build BVH from objects
    Object *world = new BVHNode(objects, 0, objects.size());

    // Setup rendering parameters
    RendererParameters params = RendererParameters::defaultParameters();
    params.imageWidth_ = imageWidth;
    params.imageHeight_ = imageHeight;
    params.samplesPerPixel_ = 100;

    // Render
    Renderer renderer(camera, params);
    renderer.render(*world, lightSphere);

    return 0;
}