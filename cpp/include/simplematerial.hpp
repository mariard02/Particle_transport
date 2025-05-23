#ifndef SIMPLEMATERIAL_HPP
#define SIMPLEMATERIAL_HPP

#include "basematerial.hpp"

class SimpleMaterial : public BaseMaterial {
protected:
    double lambda;         ///< Mean free path of the particle in the material
    double pabs;           ///< Probability of absorption upon interaction
    double k;              ///< Diffusion coefficient or parameter for Brownian motion
    double atomicMass;     ///< Atomic mass of the material
    double stoppingPower;  ///< Energy loss per unit length (relevant for charged particles)

public:
    SimpleMaterial(double lambda, double pabs, double k, double stoppingPower = 0.0, double atomicMass = -1.0)
        : BaseMaterial(), lambda(lambda), pabs(pabs), k(k), stoppingPower(stoppingPower), atomicMass(atomicMass) {}

    double getLambda(const Particle& p) const override { return lambda; }  
    double getPabs(const Particle& p) const override { return pabs; }     
    double getK(const Particle& p) const override { return k; } 
    double getAtomicMass(const Particle& p) const override {return atomicMass; }    
    double getStoppingPower(const Particle& p) const override {return stoppingPower;}
    bool hasElasticScattering(const Particle& p) const override {return atomicMass > 0.0;}

    virtual ~SimpleMaterial() = default;

    
};

#endif