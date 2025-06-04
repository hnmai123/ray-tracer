#ifndef RAYTRACER_INTERVAL_H
#define RAYTRACER_INTERVAL_H

#include "HelperFunctions.h"

class Interval {
public:
    Interval(double min, double max) : min_{min}, max_{max} {};

    double min() const { return min_; }
    double max() const { return max_; }

    double size() const { return max() - min(); }

    bool contains(double testValue) const { return (min() <= testValue) && (testValue <= max()); }
    bool surrounds(double testValue) const { return (min() < testValue) && (testValue < max()); }

    double clampValue(double inValue) const {
        if (contains(inValue)) { return inValue; }

        return (inValue < min()) ? min() : max();
    }

    static double clampValueToInterval(double inValue, Interval interval) {
        if (interval.contains(inValue)) { return inValue; }

        return (inValue < interval.min()) ? interval.min() : interval.max();
    }

    static const Interval emptyInterval;
    static const Interval infiniteInterval;

private:
    double min_ {+infinity};
    double max_ {-infinity};
};

const auto Interval::emptyInterval = Interval(+infinity, -infinity);
const auto Interval::infiniteInterval = Interval(-infinity, +infinity);

#endif //RAYTRACER_INTERVAL_H
