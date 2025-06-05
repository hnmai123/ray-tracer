#include <iostream>
#include <fstream>
#include "BVHNode.h"
#include "Renderer.h"

int main()
{
    int imageWidth = 512;
    int imageHeight = 512;
    std::string inFileName{};
    std::string outFileName{"image_7.ppm"};

    std::ofstream outFile(outFileName);

    if (!outFile.is_open())
    {
        std::cerr << "Error: Unable to open the file." << std::endl;
        return 1; // Return error code
    }

    Camera camera = Camera(
        Point3(0, 4, 10),
        Point3(0, 0, -1.5),
        512, 1.0);

    outFile << "P3\n"
            << imageWidth << " " << imageHeight << "\n255\n";

    // Create materials of diffuse and reflective types
    const Material *greenDiffuse = new Reflective(Color3(0.3, 0.8, 0.3));                          // green color
    const Material *greenGlossy = new Glossy(Color3(0.2, 0.8, 0.2), 0.1);                          // glossy green color
    const Material *redDiffuse = new PureDiffuse(Color3(0.8, 0.2, 0.2));                           // red color
    const Material *pinkMirror = new Reflective(Color3(1, 0.6, 0.8));                              // pink color
    const Material *goldGlossy = new Glossy(Color3(1.0, 0.84, 0.0), 0.5);                          // gold color with slight glossiness
    const Material *sunEmissive = new Emissive(Color3(0.9, 0.84, 0.48));                           // sun color (light source)
    const Material *caroChecker = new Checker(Color3(0.4, 0.2, 0.1), Color3(0.8, 0.6, 0.3), 10.0); // Caro checker
    const Material *dielectric = new Dielectric(1.33);                                             // Glass-like material with refractive index of water

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
    objects.push_back(new Sphere(Point3(-0.25, -0.3, -1.5), 0.2, greenDiffuse)); // Green sphere
    // Ground plane
    objects.push_back(new Plane(Point3(0, -0.5, 0), caroChecker)); // Large ground plane

    // Build BVH from objects
    Object *world = new BVHNode(objects, 0, objects.size());

    // Setup rendering parameters
    RendererParameters params = RendererParameters::defaultParameters();
    params.imageWidth_ = imageWidth;
    params.imageHeight_ = imageHeight;
    params.samplesPerPixel_ = 10;

    // Render
    Renderer renderer(camera, params);
    renderer.render(*world, lightSphere);

    return 0;
}