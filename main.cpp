#include <iostream>
#include <fstream>

#include "Camera.h"
#include "Color3.h"
#include "Ray.h"
#include "Scene.h"
#include "Material.h"
#include "BVHNode.h"

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

Color3 rayColor(const Ray &ray, const Object &world, int depth = 5)
{
    if (depth <= 0)
    {
        return Color3(0, 0, 0); // Ray bounces exhausted
    }

    auto hitRecordOpt = world.rayHit(ray, Interval(0.001, infinity));
    if (hitRecordOpt)
    {
        const HitRecord &rec = *hitRecordOpt;
        const Material *material = rec.surfaceMaterial();

        if (auto reflected = material->scatter(ray, rec))
        {
            Color3 incoming = rayColor(*reflected, world, depth - 1);
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
    const Material *diffuse = new PureDiffuse(Color3(0.2, 0.9, 0.9)); // light cyan color
    const Material *mirror = new Reflective(Color3(0.2, 0.2, 0.4)); // dark blue color
    const Material *glossy = new Glossy(Color3(0.8, 0.6, 0.2), 0.5); // glossy yellow color
    
    // Create objects list for BVH tree
    std::vector<Object*> objects;

    // Spheres
    objects.push_back(new Sphere(Point3(0.0, 0, -1.0), 0.5, glossy));    // Glossy center
    
    // Ground plane
    objects.push_back(new Plane(Point3(0, -0.5, 0), mirror));

    // Build BVH from objects
    Object* world = new BVHNode(objects, 0, objects.size());

    // Render the scene
    int samplesPerPixel = 500; // Number of samples per pixel

    for (int j = 0; j < imageHeight; j++)
    {
        for (int i = 0; i < imageWidth; i++)
        {
            Color3 pixelColor;
            for (int sample = 0; sample < samplesPerPixel; sample++)
            {
                // Get a ray for the current pixel
                auto ray = camera.getRay(i + randomDouble0to1(), j + randomDouble0to1());

                // Accumulate color from the ray
                pixelColor = pixelColor + rayColor(ray, *world, 10);
            }

            Color3 finalColor = pixelColor.correctedAverage(samplesPerPixel); // Average color for the pixel

            outFile << finalColor.r() << ' ' << finalColor.g() << ' ' << finalColor.b() << '\n';
        }
        updateProgressBar(j, imageHeight);
    }

    outFile.close();

    return 0;
}