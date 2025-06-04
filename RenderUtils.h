#ifndef RAYTRACER_RENDERUTILS_H
#define RAYTRACER_RENDERUTILS_H
#include "Color3.h"
#include "Camera.h"
#include "Scene.h"
#include <thread>

inline void updateProgressBar(int rowsDone, int imageHeight)
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

inline Color3 rayColor(const Ray &ray, const Object &world, int depth = 10, const Sphere *lightSource = nullptr)
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
            int lightSamples = 10; // Number of samples for direct light
            for (int i = 0; i < lightSamples; ++i)
            {
                Point3 lightSamplePoint = lightSource->randomPointOnSurface(); // Sample a point on the light source
                Vector3 lightDirection = (lightSamplePoint - rec.hitPoint()).unitVector();
                double lightDistance = (lightSamplePoint - rec.hitPoint()).length();
                Ray shadowRay(rec.hitPoint(), lightDirection);
                bool inShadow = world.rayHit(shadowRay, Interval(0.01, lightDistance - 0.01)).has_value();
                if (!inShadow)
                {
                    // If not in shadow, add direct light contribution
                    double cosine = std::max(0.0, rec.surfaceNormal().dot(lightDirection));
                    double attenuation = 1.0 / (lightDistance * lightDistance); // Simple attenuation
                    Color3 lightIntensity = lightSource->material()->emittedColor() * attenuation * cosine;
                    directLight += baseColor * lightIntensity; // Direct light contribution
                }
            }
            directLight /= static_cast<double>(lightSamples); // Average direct light contribution
        }
        return emittedColor + indirectLight + directLight; // Combine emitted, indirect, and direct light contributions
    }

    // Background gradient
    auto unitDirection = ray.direction().unitVector();
    auto verticalBlendFactor = 0.5 * (unitDirection.y() + 1.0);
    Color3 skyTopColor = Color3(0.2, 0.5, 0.95);     // Strong at top
    Color3 skyBottomColor = Color3(0.85, 0.95, 1.0); // Light at bottom

    return skyBottomColor * (1.0 - verticalBlendFactor) + skyTopColor * verticalBlendFactor;
    // return Color3(0, 0, 0); // Night sky background
}

inline void renderRows(int startRow, int endRow, int imageWidth, int imageHeight, int samplesPerPixel,
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

inline void renderMultithread(int imageWidth, int imageHeight, int samplesPerPixel,
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

#endif // RAYTRACER_RENDERUTILS_H