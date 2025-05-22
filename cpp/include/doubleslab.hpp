#ifndef DOUBLESLAB_HPP
#define DOUBLESLAB_HPP

#include "particle.hpp"
#include "regularslab.hpp"
#include <memory>

/**
 * @brief A composite material made of two adjacent slabs with different properties.
 * 
 * The DoubleSlab class represents a material composed of two RegularSlabs placed next to each other.
 * It inherits from BaseMaterial but internally delegates behavior to its two constituent slabs.
 */
class DoubleSlab : public BaseMaterial {
private:
    double totalLength; ///< Total length of the composite slab
    double ratio;       ///< Ratio of the total length occupied by material1
    double xinit;       ///< Starting x-position of the double slab

    RegularSlab material1; ///< First slab (occupies the first portion of the length)
    RegularSlab material2; ///< Second slab (occupies the remaining portion)

public:
    /**
     * @brief Constructs a DoubleSlab with two materials and layout information.
     * 
     * @param lambda1, pabs1, k1: Parameters for material1
     * @param lambda2, pabs2, k2: Parameters for material2
     * @param totalLength: Total length of the slab (material1 + material2)
     * @param xinit: Initial x-coordinate of the slab
     * @param ratio: Fraction of the totalLength occupied by material1
     * @param stoppingPower1, stoppingPower2: Energy loss per unit distance in each material
     * @param atomicMass1, atomicMass2: Atomic masses of each material (used for scattering)
     */
    DoubleSlab(double lambda1, double pabs1, double k1, 
               double lambda2, double pabs2, double k2, 
               double totalLength, double xinit, double ratio, 
               double stoppingPower1 = 0.0, double stoppingPower2 = 0.0,
               double atomicMass1 = -1.0, double atomicMass2 = -1.0)
    : totalLength(totalLength), 
      ratio(ratio), 
      xinit(xinit),
      material1(lambda1, pabs1, k1, totalLength * ratio, xinit, stoppingPower1, atomicMass1),
      material2(lambda2, pabs2, k2, totalLength * (1 - ratio), xinit + totalLength * ratio, stoppingPower2, atomicMass2) {}

    /// @return Length of the first material slab
    double getLength1() const { return totalLength * ratio; }

    /// @return Length of the second material slab
    double getLength2() const { return totalLength * (1 - ratio); }

    /// @return Reference to the first material
    const RegularSlab& getMaterial1() const { return material1; }

    /// @return Reference to the second material
    const RegularSlab& getMaterial2() const { return material2; }

    /// @return Total length of the combined slab
    double getTotalLength() const { return totalLength; }

    /// @return Ratio of the total length occupied by the first material
    double getRatio() const { return ratio; }

    /// @return Position of the double slab
    double getXInit() const {return xinit;}

    /**
     * @brief Check if the particle is within the bounds of the double slab.
     * 
     * @param particle The particle whose position is checked.
     * @return true if the particle lies within the x-range of the slab
     */
    bool isWithinBounds(const Particle& particle) const override;

    virtual ~DoubleSlab() = default;
};

#endif // DOUBLESLAB_HPP
