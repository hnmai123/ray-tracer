#include <iostream>
#include <fstream>

#include "Camera.h"
#include "Color3.h"
#include "Ray.h"
#include "Scene.h"

void updateProgressBar(int rowsDone, int imageHeight){
    // To get to a percentage first subtract rowsDone from the height, because rowsDone is descending,
    float percentage = ((float)(imageHeight - rowsDone) / imageHeight) * 100;
    std::cout << "[";
    for(int i = 0; i < 50; i++){
        if(i < (percentage / 2)){ // percentage needs to be divided by 2 because the percentage bar is 50 width
            std::cout << "#";
        }else{
            std::cout << " ";
        }
    }
    std::cout << "] " << percentage << "% \r" << std::flush;
}

Color3 rayColor(const Ray& r, const Scene& scene) {
    Interval rayInterval(0.001, infinity); // Avoid self-intersection
    auto hit = scene.rayHit(r, rayInterval);

    if (hit) {
        // Simple shading: map normal vector to color
        Vector3 normal = hit->surfaceNormal();
        // Convert normal from [-1,1] to [0,1] for color
        return Color3(normal.x() + 1, normal.y() + 1, normal.z() + 1) * 0.5;
    }

    auto unitDirection = r.direction().unitVector();
    auto lerpFactor = 0.5 * (unitDirection.y() + 1.0);

    auto white = Color3(1.0, 1.0, 1.0);
    auto blue = Color3(0.5, 0.7, 1.0);

    return ((1.0 - lerpFactor) * white) + (lerpFactor * blue);
}

int main() {
    int imageWidth = 512;
    int imageHeight = 512;
    std::string inFileName {};
    std::string outFileName {"image_7.ppm"};

    std::ofstream outFile(outFileName);

    if (!outFile.is_open()) {
        std::cerr << "Error: Unable to open the file." << std::endl;
        return 1; // Return error code
    }

    Camera camera = Camera();

    // Create a scene with a sphere and a plane
    Scene scene;
    scene.add(new Sphere()); // Add a sphere object
    scene.add(new Plane()); // Add a plane object

    outFile << "P3\n" << imageWidth << " " << imageHeight << "\n255\n";

    for (int j = 0; j < imageHeight; j++) {
        for (int i = 0; i < imageWidth; i++) {
            auto ray = camera.getRay(i, j);

            auto pixelColor = rayColor(ray, scene);

            outFile << pixelColor.r() << ' ' << pixelColor.g() << ' ' << pixelColor.b() << '\n';
        }
        updateProgressBar(j, imageHeight);
    }

    outFile.close();

    return 0;
}