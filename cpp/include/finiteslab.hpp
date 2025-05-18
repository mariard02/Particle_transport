#ifndef FINITESLAB_HPP
#define FINITESLAB_HPP

#include "basematerial.hpp"

class FiniteSlab : public BaseMaterial {
private:
    double xlength;
    double ylength;
    double zlength;

public:
    FiniteSlab(double lambda, double pabs, double k, double xlength, double ylength, double zlength, double atomicMass = -1.0)
        : BaseMaterial(lambda, pabs, k, atomicMass), xlength(xlength), ylength(ylength), zlength(zlength) {}

    double getXLength() const {return xlength;}
    double getYLength() const {return ylength;}
    double getZLength() const {return zlength;}

    virtual ~FiniteSlab() = default;

    bool isWithinBounds(const Particle& particle) const override;
    
};

#endif