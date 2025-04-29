#ifndef BASEMATERIAL_HPP
#define BASEMATERIAL_HPP

class Neutron; 

class BaseMaterial {
protected:
    double lambda;
    double pabs;
    double k;

public:
    BaseMaterial(double lambda, double pabs, double k)
        : lambda(lambda), pabs(pabs), k(k) {}

    virtual ~BaseMaterial() = default;

    double getLambda() const { return lambda; }
    double getPabs() const { return pabs; }
    double getK() const { return k; }

    virtual bool isWithinBounds(const Neutron& particle) const = 0;
};

#endif
