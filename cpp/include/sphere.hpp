#ifndef SPHERE_HPP
#define SPHERE_HPP

#include "basematerial.hpp"
#include <array>

class Sphere : public BaseMaterial {
private:
    double radius;

public:
    Sphere(double lambda, double pabs, double k, double radius, double stoppingPower = 0.0, double atomicMass = -1.0)
        : BaseMaterial(lambda, pabs, k, atomicMass, stoppingPower), radius(radius) {}

    double getRadius() const {return radius;}

    virtual ~Sphere() = default;

    bool isWithinBounds(const Particle& particle) const override;
};

#endif
