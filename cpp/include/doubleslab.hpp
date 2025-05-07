#ifndef DOUBLESLAB_HPP
#define DOUBLESLAB_HPP

#include "neutron.hpp"
#include "regularslab.hpp"
#include <memory>

class DoubleSlab : public BaseMaterial {
private:
    double totalLength; 
    double ratio;
    double xinit;
    RegularSlab material1; 
    RegularSlab material2; 

public:
    DoubleSlab(double lambda1, double pabs1, double k1, 
           double lambda2, double pabs2, double k2, 
           double totalLength, double xinit, double ratio, 
           double atomicMass1 = -1.0, double atomicMass2 = -1.0)
    : totalLength(totalLength), 
      ratio(ratio), 
      xinit(xinit),
      material1(lambda1, pabs1, k1, totalLength * ratio, xinit, atomicMass1),
      material2(lambda2, pabs2, k2, totalLength * (1 - ratio), xinit + totalLength * ratio, atomicMass2) {}

    double getLength1() const { return totalLength * ratio; }
    double getLength2() const { return totalLength * (1 - ratio); }

    RegularSlab getMaterial1() const { return material1; }
    RegularSlab getMaterial2() const { return material2; }

    double getTotalLength() const { return totalLength; }

    double getRatio() const { return ratio; }

    bool isWithinBounds(const Neutron& particle) const override;

    virtual ~DoubleSlab() = default;
};

#endif
