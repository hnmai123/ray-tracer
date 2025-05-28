#include <iostream>
#include <fstream>

#include "Camera.h"
#include "Color3.h"
#include "Ray.h"
#include "Scene.h"
#include "Material.h"

void updateProgressBar(int rowsDone, int imageHeight)
{
    // To get to a percentage first subtract rowsDone from the height, because rowsDone is descending,
    float percentage = ((float)(imageHeight - rowsDone) / imageHeight) * 100;
    std::cout << "[";
    for (int i = 0; i < 50; i++)
    {
        if (i < (percentage / 2))
        { // percentage needs to be divided by 2 because the percentage bar is 50 width
            std::cout << "#";
        }
        else
        {
            std::cout << " ";
        }
    }
    std::cout << "] " << percentage << "% \r" << std::flush;
}

Color3 rayColor(const Ray &ray, const Scene &scene, int depth = 10)
{
    if (depth <= 0)
    {
        return Color3(0, 0, 0); // Ray bounces exhausted
    }

    auto hitRecordOpt = scene.rayHit(ray, Interval(0.001, infinity));
    if (hitRecordOpt)
    {
        const HitRecord &rec = *hitRecordOpt;
        const Material *material = rec.surfaceMaterial();

        if (auto reflected = material->scatter(ray, rec))
        {
            Color3 incoming = rayColor(*reflected, scene, depth - 1);
            return material->color() * incoming;
        }
        else
        {
            return Color3(0, 0, 0); // Absorbed
        }
    }

    // Background gradient
    auto unitDirection = ray.direction().unitVector();
    auto verticalBlendFactor = 0.5 * (unitDirection.y() + 1.0);
    Color3 skyBottomColor = Color3(1.0, 1.0, 1.0); // White at bottom
    Color3 skyTopColor = Color3(0.5, 0.7, 1.0);    // Light blue at top

    return skyBottomColor * (1.0 - verticalBlendFactor) + skyTopColor * verticalBlendFactor;
}

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

    Camera camera = Camera();

    outFile << "P3\n"
            << imageWidth << " " << imageHeight << "\n255\n";

    // Create materials of diffuse and reflective types
    const Material *diffuse = new PureDiffuse(Color3(0.95, 0.95, 1.0)); // Light gray color
    const Material *mirror = new Reflective(Color3(0.5, 0.5, 0.5)); // Medium gray color
    
    // Create a scene with a sphere and a plane
    Scene scene;
    scene.add(new Sphere(Point3(0, 0, -1), 0.5, diffuse));
    scene.add(new Plane(Point3(0, -0.5, 0), mirror));

    for (int j = 0; j < imageHeight; j++)
    {
        for (int i = 0; i < imageWidth; i++)
        {
            auto ray = camera.getRay(i, j);

            auto pixelColor = rayColor(ray, scene, 10);

            outFile << pixelColor.r() << ' ' << pixelColor.g() << ' ' << pixelColor.b() << '\n';
        }
        updateProgressBar(j, imageHeight);
    }

    outFile.close();

    return 0;
}