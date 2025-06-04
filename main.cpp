#include <iostream>
#include <fstream>

#include "Camera.h"
#include "Color3.h"
#include "Ray.h"
#include "Scene.h"
#include "Material.h"
#include "BVHNode.h"
#include <thread>
#include <atomic>

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

Color3 rayColor(const Ray &ray, const Object &world, int depth = 10, const Sphere *lightSource = nullptr)
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

        Color3 baseColor = material->color();
        if (auto checkerMat = dynamic_cast<const Checker *>(material))
        {
            baseColor = checkerMat->checkerColorAt(rec.hitPoint());
        }
        const Color3 emittedColor = material->emittedColor();

        // Calculate indirect light contribution
        Color3 indirectLight = Color3(0, 0, 0);
        if (auto reflected = material->scatter(ray, rec))
        {
            Color3 incoming = rayColor(*reflected, world, depth - 1, lightSource);
            double cosine = std::max(0.0, rec.surfaceNormal().dot(reflected->direction().unitVector()));
            indirectLight = baseColor * incoming * (cosine * (1.0 / pi)); // Apply Lambertian reflectance
        }
        
        // Calculate direct light contribution if a light source is provided
        Color3 directLight = Color3(0, 0, 0);
        if (lightSource != nullptr)
        {
            Vector3 lightDirection = (lightSource->centre() - rec.hitPoint()).unitVector();
            Ray shadowRay(rec.hitPoint(), lightDirection);

            double lightDistance = (lightSource->centre() - rec.hitPoint()).length();
            bool inShadow = world.rayHit(shadowRay, Interval(0.01, lightDistance - 0.01)).has_value();

            if (!inShadow)
            {
                // If not in shadow, add direct light contribution
                double cosine = std::max(0.0, rec.surfaceNormal().dot(lightDirection));
                double attenuation = 1.0 / (lightDistance * lightDistance); // Simple attenuation
                Color3 lightIntensity = lightSource->material()->emittedColor() * attenuation * cosine;
                directLight = baseColor * lightIntensity; // Direct light contribution
            }
        }
        return emittedColor + indirectLight + directLight; // Combine emitted, indirect, and direct light contributions
    }

    // Background gradient
    auto unitDirection = ray.direction().unitVector();
    auto verticalBlendFactor = 0.5 * (unitDirection.y() + 1.0);
    Color3 skyBottomColor = Color3(0.1, 0.1, 0.1); // Dark gray at bottom
    Color3 skyTopColor = Color3(0.3, 0.4, 0.6);    // Light blue at top

    return skyBottomColor * (1.0 - verticalBlendFactor) + skyTopColor * verticalBlendFactor;
    // return Color3(0, 0, 0); // Night sky background
}

void renderRows(int startRow, int endRow, int imageWidth, int imageHeight, int samplesPerPixel,
                const Camera &camera, const Object &world, std::vector<Color3> &frameBuffer, 
                std::atomic<int> &rowsCompleted, const Sphere *lightSource)
{
    for (int j = startRow; j < endRow; j++)
    {
        for (int i = 0; i < imageWidth; i++)
        {
            Color3 pixelColor;
            for (int sample = 0; sample < samplesPerPixel; sample++)
            {
                // Get a ray for the current pixel
                auto ray = camera.getRay(i + randomDouble0to1(), j + randomDouble0to1());

                // Accumulate color from the ray
                pixelColor += rayColor(ray, world, 10, lightSource);
            }

            Color3 finalColor = pixelColor.correctedAverage(samplesPerPixel); // Average color for the pixel
            frameBuffer[j * imageWidth + i] = finalColor;
        }
        int done = ++rowsCompleted;
        updateProgressBar(done, imageHeight);
    }
}

void renderMultithread(int imageWidth, int imageHeight, int samplesPerPixel,
                       const Camera &camera, const Object &world, std::atomic<int> &rowsCompleted,
                       std::vector<Color3> &frameBuffer, const Sphere *lightSource)
{
    unsigned int threadCount = std::thread::hardware_concurrency();
    std::cout << "Rendering with " << threadCount << " threads...\n";
    std::vector<std::thread> threads;
    int rowsPerThread = imageHeight / threadCount;

    for (int t = 0; t < threadCount; ++t)
    {
        int startRow = t * rowsPerThread;
        int endRow = (t == threadCount - 1) ? imageHeight : startRow + rowsPerThread;
        threads.emplace_back(renderRows, startRow, endRow, imageWidth, imageHeight, samplesPerPixel,
                             std::ref(camera), std::ref(world), std::ref(frameBuffer), 
                             std::ref(rowsCompleted), lightSource);
    }

    for (auto &thread : threads)
        thread.join();
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

    Camera camera = Camera(
        Point3(0, 30, 30),  // Camera high above the scene
        Point3(0, 0, -1.5), // Looking towards the center of the scene
        512, 1.0            // Aspect ratio of 1.0 for square image
    );

    outFile << "P3\n"
            << imageWidth << " " << imageHeight << "\n255\n";

    // Create materials of diffuse and reflective types
    const Material *greenDiffuse = new Reflective(Color3(0.3, 0.8, 0.3));  // green color
    const Material *greenGlossy = new Glossy(Color3(0.2, 0.8, 0.2), 0.1); // glossy green color
    const Material *redDiffuse = new PureDiffuse(Color3(0.8, 0.2, 0.2));  // red color
    const Material *pinkMirror = new Reflective(Color3(1, 0.6, 0.8));  // pink color
    const Material *goldGlossy = new Glossy(Color3(1.0, 0.84, 0.0), 0.1); // gold color with slight glossiness
    const Material *sunEmissive = new Emissive(Color3(1.0, 0.95, 0.6));   // sun color (light source)
    const Material* caroChecker = new Checker(Color3(0.4, 0.2, 0.1), Color3(0.8, 0.6, 0.3), 10.0);;  // Caro checker
    const Material *glass = new Dialectric(1.5); // Glass material with refractive index of 1.5

    // Create objects list for BVH tree
    std::vector<Object *> objects;

    // Emissive sphere (light source)
    Sphere* lightSphere = new Sphere(Point3(0.0, 0.2, -1.5), 0.3, sunEmissive); // Light source sphere
    objects.push_back(lightSphere); 

    // Add spheres with different materials
    objects.push_back(new Sphere(Point3(-0.5, -0.2, -1.3), 0.3, redDiffuse)); // 1x1
    objects.push_back(new Sphere(Point3(0.5, -0.2, -1.3), 0.3, glass)); // 1x2
    objects.push_back(new Sphere(Point3(-0.5, -0.2, -1.7), 0.3, goldGlossy)); // 2x1
    objects.push_back(new Sphere(Point3(0.5, -0.2, -1.7), 0.3, pinkMirror)); // 2x2

    // Ground plane
    objects.push_back(new Plane(Point3(0, -0.5, 0), caroChecker)); // Large ground plane
    
    // Build BVH from objects
    Object *world = new BVHNode(objects, 0, objects.size());

    // Render the scene
    int samplesPerPixel = 50; // Number of samples per pixel

    // Multithread rendering
    std::vector<Color3> frameBuffer(imageWidth * imageHeight);
    std::atomic<int> rowsCompleted = 0;

    renderMultithread(imageWidth, imageHeight, samplesPerPixel, camera, *world, rowsCompleted, frameBuffer, lightSphere);

    // Write the frame buffer to the output file
    for (int j = 0; j < imageHeight; j++)
    {
        for (int i = 0; i < imageWidth; i++)
        {
            Color3 color = frameBuffer[j * imageWidth + i];
            outFile << color.r() << ' ' << color.g() << ' ' << color.b() << '\n';
        }
    }
    outFile.close();

    return 0;
}