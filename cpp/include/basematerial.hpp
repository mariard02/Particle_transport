#ifndef BASEMATERIAL_HPP
#define BASEMATERIAL_HPP

class Particle;

class BaseMaterial {
protected:
    double lambda;
    double pabs;
    double k;
    double atomicMass;
    double stoppingPower;  

public:
    BaseMaterial(double lambda, double pabs, double k, double atomicMass = -1.0, double stoppingPower = 0.01)
        : lambda(lambda), pabs(pabs), k(k), atomicMass(atomicMass), stoppingPower(stoppingPower) {}

    BaseMaterial() : lambda(0.0), pabs(0.0), k(0.0), atomicMass(-1.0), stoppingPower(10.0) {}

    virtual ~BaseMaterial() = default;

    double getLambda() const { return lambda; }
    double getPabs() const { return pabs; }
    double getK() const { return k; }

    virtual bool isWithinBounds(const Particle& particle) const = 0;

    bool hasElasticScattering() const { return atomicMass > 0.0; }
    double getAtomicMass() const { return atomicMass; }

    double getStoppingPower() const { return stoppingPower; }
};

#endif
