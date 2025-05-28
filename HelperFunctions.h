//
// Created by Richard Skarbez on 5/7/24.
//

#ifndef RAYTRACER_HELPERFUNCTIONS_H
#define RAYTRACER_HELPERFUNCTIONS_H

#include <cmath>
#include <iostream>
#include <optional>
#include <random>

const double pi = 3.1415926535897932385;
const double infinity = std::numeric_limits<double>::infinity();

double degreesToRadians(double degrees) {
    return (degrees * pi) / 180.0;
}

double radiansToDegrees(double radians) {
    return (180.0 * radians) / pi;
}

// Random number generators

double randomDouble(double minimum, double maximum) {
    std::random_device randomDevice;
    std::mt19937 generator (randomDevice());
    std::uniform_real_distribution<double> distribution(minimum, maximum);
    return distribution(generator);
}

double randomDouble0to1() {
    return randomDouble(0.0, 1.0);
}

int randomInt(int minimum, int maximum) {
    std::random_device randomDevice;
    std::mt19937 generator (randomDevice());
    std::uniform_int_distribution<int> distribution(minimum, maximum);
    return distribution(generator);
}

double randomInt0to255() {
    return randomInt(0, 255);
}

#endif //RAYTRACER_HELPERFUNCTIONS_H
