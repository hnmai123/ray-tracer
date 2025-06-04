#ifndef RAYTRACER_HELPERFUNCTIONS_H
#define RAYTRACER_HELPERFUNCTIONS_H

#include <cmath>
#include <iostream>
#include <optional>
#include <random>

static thread_local std::mt19937 rng(std::random_device{}());

const double pi = 3.1415926535897932385;
const double infinity = std::numeric_limits<double>::infinity();

double degreesToRadians(double degrees)
{
    return (degrees * pi) / 180.0;
}

double radiansToDegrees(double radians)
{
    return (180.0 * radians) / pi;
}

// Random number generators

double randomDouble(double minimum, double maximum)
{
    std::uniform_real_distribution<double> distribution(minimum, maximum);
    return distribution(rng);
}

double randomDouble0to1()
{
    return randomDouble(0.0, 1.0);
}

int randomInt(int minimum, int maximum)
{
    std::uniform_int_distribution<int> distribution(minimum, maximum);
    return distribution(rng);
}

double randomInt0to255()
{
    return randomInt(0, 255);
}

#endif // RAYTRACER_HELPERFUNCTIONS_H
