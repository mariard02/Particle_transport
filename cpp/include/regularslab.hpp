#ifndef REGULARSLAB_HPP
#define REGULARSLAB_HPP

#include "basematerial.hpp"

class RegularSlab : public BaseMaterial {
private:
    double length;
    double xinit;

public:
    RegularSlab(double lambda, double pabs, double k, double length, double xinit, double atomicMass = -1.0)
        : BaseMaterial(lambda, pabs, k, atomicMass), length(length), xinit(xinit) {}

    double getLength() const {return length;}

    virtual ~RegularSlab() = default;

    bool isWithinBounds(const Neutron& particle) const override;
    
};

#endif