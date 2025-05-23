#ifndef REGULARSLAB_HPP
#define REGULARSLAB_HPP

#include "basematerial.hpp"
#include "simplematerial.hpp"

class RegularSlab : public SimpleMaterial {
private:
    double length;
    double xinit;

public:
    RegularSlab(double lambda, double pabs, double k, double length, double xinit, double stoppingPower = 0.0, double atomicMass = -1.0)
        : SimpleMaterial(lambda, pabs, k, stoppingPower, atomicMass), length(length), xinit(xinit) {}

    double getLength() const {return length;}
    double getXInit() const {return xinit;}

    virtual ~RegularSlab() = default;

    bool isWithinBounds(const Particle& particle) const override;
    
};

#endif