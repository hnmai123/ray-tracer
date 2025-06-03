//
// Created by Richard Skarbez on 5/7/24.
//

#ifndef RAYTRACER_CAMERA_H
#define RAYTRACER_CAMERA_H

#include "Vector3.h"
#include "Ray.h"

class Camera {
public:
    Camera(const Point3& cameraPosition = Vector3(0,0,0),
           const Point3& imagePlaneCentre = Vector3(0, 0, -1),
           double imageHeight = 512,
           double aspectRatio = 1) {
        double viewportHeight = 2.0;
        double imageWidth = imageHeight * aspectRatio;
        double viewportWidth = viewportHeight * aspectRatio;
        cameraCentre_ = cameraPosition;
        auto lookAtVector = imagePlaneCentre - cameraPosition;
        auto viewportU = Vector3(viewportWidth, 0.0, 0.0);
        auto viewportV = Vector3(0.0, -viewportHeight, 0.0);

        horizontalStep_ = viewportU / imageWidth;
        verticalStep_ = viewportV / imageHeight;

        auto viewportULCorner = cameraCentre_ + lookAtVector - (viewportU / 2.0) - (viewportV / 2.0);
        pixel00_ = viewportULCorner + (0.5 * horizontalStep_) + (0.5 * verticalStep_);

    }

    Ray getRay(double x, double y) const {
        return Ray(cameraCentre_, (pixel00_ + (horizontalStep_ * x) + (verticalStep_ * y)) - cameraCentre_);
    }
private:
    Point3 cameraCentre_ {0.0, 0.0, 0.0};
    Point3 pixel00_ {0.0, 0.0, 0.0};
    Vector3 horizontalStep_ {1.0, 0.0, 0.0};
    Vector3 verticalStep_ {0.0, 1.0, 0.0};
};

#endif //RAYTRACER_CAMERA_H
