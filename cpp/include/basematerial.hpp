#ifndef BASEMATERIAL_HPP
#define BASEMATERIAL_HPP

class Neutron; 

class BaseMaterial {
protected:
    double lambda;
    double pabs;
    double k;
    double atomicMass;

public:
    BaseMaterial(double lambda, double pabs, double k, double atomicMass = -1.0)
        : lambda(lambda), pabs(pabs), k(k), atomicMass(atomicMass) {}

    BaseMaterial() : lambda(0.0), pabs(0.0), k(0.0), atomicMass(-1.0) {}

    virtual ~BaseMaterial() = default;

    double getLambda() const { return lambda; }
    double getPabs() const { return pabs; }
    double getK() const { return k; }

    virtual bool isWithinBounds(const Neutron& particle) const = 0;

    bool hasElasticScattering() const { return atomicMass > 0.0; }
    double getAtomicMass() const { return atomicMass; }
};

#endif
