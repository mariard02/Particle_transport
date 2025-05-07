#ifndef SPHERE_HPP
#define SPHERE_HPP

#include "basematerial.hpp"
#include <array>

class Sphere : public BaseMaterial {
private:
    double radius;
    std::array<double, 3> center;

public:
    Sphere(double lambda, double pabs, double k, double radius, double atomicMass = -1.0)
        : BaseMaterial(lambda, pabs, k, atomicMass), radius(radius) {}

    double getRadius() const {return radius;}
    const std::array<double, 3> getCenter() const { return center; }

    virtual ~Sphere() = default;

    bool isWithinBounds(const Neutron& particle) const override;
};

#endif
