#ifndef RAYTRACER_RENDERER_H
#define RAYTRACER_RENDERER_H

#include <fstream>

#include "Camera.h"
#include "Color3.h"
#include "Scene.h"
#include <thread>

class RendererParameters
{
public:
    int imageWidth_{512};
    int imageHeight_{512};
    int samplesPerPixel_{10};
    int maximumRecursionDepth_{25};
    Color3 backgroundColor_{0.0, 0.0, 0.0};
    std::string fileName_{"image.ppm"};
    static RendererParameters defaultParameters()
    {
        return RendererParameters();
    }
};

class Renderer
{
public:
    Renderer(const Camera &camera, const RendererParameters &params) : camera_(camera), params_(params), frameBuffer_(params.imageWidth_ * params.imageHeight_),
                                                                       rowsCompleted_(0) {}
    void render(const Object &world, const Sphere *lightSource = nullptr)
    {
        std::cout << "Rendering with " << std::thread::hardware_concurrency() << " threads...\n";
        renderMultithread(world, lightSource);
        writeOutput(params_.fileName_);
    }

private:
    Camera camera_;
    RendererParameters params_;
    std::vector<Color3> frameBuffer_;
    std::atomic<int> rowsCompleted_;

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

    void updateProgressBar(int rowsDone)
    {
        float percent = ((float)rowsDone / params_.imageHeight_) * 100.0f;
        std::cout << "[";
        for (int i = 0; i < 50; ++i)
            std::cout << (i < percent / 2 ? "#" : " ");
        std::cout << "] " << percent << "%\r" << std::flush;
    }

    void renderRows(int startRow, int endRow, const Object &world, const Sphere *lightSource)
    {
        for (int j = startRow; j < endRow; ++j)
        {
            for (int i = 0; i < params_.imageWidth_; ++i)
            {
                Color3 pixelColor;
                for (int s = 0; s < params_.samplesPerPixel_; ++s)
                {
                    Ray ray = camera_.getRay(i + randomDouble0to1(), j + randomDouble0to1());
                    pixelColor += rayColor(ray, world, params_.maximumRecursionDepth_, lightSource);
                }
                frameBuffer_[j * params_.imageWidth_ + i] = pixelColor.correctedAverage(params_.samplesPerPixel_);
            }
            updateProgressBar(++rowsCompleted_);
        }
    }

    void renderMultithread(const Object &world, const Sphere *lightSource)
    {
        int threadCount = std::thread::hardware_concurrency();
        std::vector<std::thread> threads;
        int rowsPerThread = params_.imageHeight_ / threadCount;

        for (int t = 0; t < threadCount; ++t)
        {
            int start = t * rowsPerThread;
            int end = (t == threadCount - 1) ? params_.imageHeight_ : start + rowsPerThread;
            threads.emplace_back(&Renderer::renderRows, this, start, end, std::ref(world), lightSource);
        }

        for (auto &thread : threads)
            thread.join();
    }

    void writeOutput(const std::string &filename)
    {
        std::ofstream outFile(filename);
        outFile << "P3\n"
                << params_.imageWidth_ << " " << params_.imageHeight_ << "\n255\n";
        for (int j = 0; j < params_.imageHeight_; ++j)
        {
            for (int i = 0; i < params_.imageWidth_; ++i)
            {
                const Color3 &color = frameBuffer_[j * params_.imageWidth_ + i];
                outFile << color.r() << ' ' << color.g() << ' ' << color.b() << '\n';
            }
        }
        outFile.close();
    }
};

#endif // RAYTRACER_RENDERER_H
