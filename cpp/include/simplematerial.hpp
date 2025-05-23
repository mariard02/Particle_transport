#ifndef SIMPLEMATERIAL_HPP
#define SIMPLEMATERIAL_HPP

#include "basematerial.hpp"

class SimpleMaterial : public BaseMaterial {
private:

public:
    SimpleMaterial(double lambda, double pabs, double k, double stoppingPower = 0.0, double atomicMass = -1.0)
        : BaseMaterial(lambda, pabs, k, atomicMass, stoppingPower) {}

    double getLambda(const Particle& p) const override { return lambda; }  
    double getPabs(const Particle& p) const override { return pabs; }     
    double getK(const Particle& p) const override { return k; } 
    double getAtomicMass(const Particle& p) const override {return atomicMass; }    
    double getStoppingPower(const Particle& p) const override {return stoppingPower;}
    bool hasElasticScattering(const Particle& p) const override {return atomicMass > 0.0;}

    virtual ~SimpleMaterial() = default;

    
};

#endif