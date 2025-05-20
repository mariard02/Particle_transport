#ifndef REGULARSLAB_HPP
#define REGULARSLAB_HPP

#include "basematerial.hpp"

class RegularSlab : public BaseMaterial {
private:
    double length;
    double xinit;

public:
    RegularSlab(double lambda, double pabs, double k, double length, double xinit, double stoppingPower = 0.0, double atomicMass = -1.0)
        : BaseMaterial(lambda, pabs, k, atomicMass, stoppingPower), length(length), xinit(xinit) {}

    double getLength() const {return length;}
    double getXInit() const {return xinit;}

    virtual ~RegularSlab() = default;

    bool isWithinBounds(const Particle& particle) const override;
    
};

#endif