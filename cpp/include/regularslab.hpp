#ifndef REGULARSLAB_HPP
#define REGULARSLAB_HPP

#include "basematerial.hpp"

class RegularSlab : public BaseMaterial {
private:
    double length;

public:
    RegularSlab(double lambda, double pabs, double k, double length)
        : BaseMaterial(lambda, pabs, k), length(length) {}

    double getLength() const {return length;}

    virtual ~RegularSlab() = default;

    bool isWithinBounds(const Neutron& particle) const override;
    
};

#endif
