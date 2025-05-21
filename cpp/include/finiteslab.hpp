#ifndef FINITESLAB_HPP
#define FINITESLAB_HPP

#include "basematerial.hpp"

/**
 * @brief Represents a finite 3D slab of material with rectangular dimensions.
 * 
 * This class models a bounded volume of material in 3D space. It inherits from BaseMaterial
 * and defines a box-shaped region within which particles may interact with the material.
 */
class FiniteSlab : public BaseMaterial {
private:
    double xlength;  ///< Length of the slab along the x-axis
    double ylength;  ///< Length of the slab along the y-axis
    double zlength;  ///< Length of the slab along the z-axis

public:
    /**
     * @brief Constructor for FiniteSlab.
     * 
     * @param lambda Interaction length (mean free path)
     * @param pabs Absorption probability
     * @param k Scattering coefficient
     * @param xlength Size of the slab along the x-axis
     * @param ylength Size of the slab along the y-axis
     * @param zlength Size of the slab along the z-axis
     * @param stoppingPower Energy loss per unit length (optional)
     * @param atomicMass Atomic mass of the material (for elastic scattering, optional)
     */
    FiniteSlab(double lambda, double pabs, double k, 
               double xlength, double ylength, double zlength, 
               double stoppingPower = 0.0, double atomicMass = -1.0)
        : BaseMaterial(lambda, pabs, k, atomicMass, stoppingPower), 
          xlength(xlength), ylength(ylength), zlength(zlength) {}

    /// @return Length of the slab along the x-axis
    double getXLength() const { return xlength; }

    /// @return Length of the slab along the y-axis
    double getYLength() const { return ylength; }

    /// @return Length of the slab along the z-axis
    double getZLength() const { return zlength; }

    /// Destructor (default)
    virtual ~FiniteSlab() = default;

    /**
     * @brief Checks if a particle is within the volume of the slab.
     * 
     * @param particle The particle whose position is to be tested.
     * @return true if the particle lies within the defined 3D box.
     */
    bool isWithinBounds(const Particle& particle) const override;
};

#endif // FINITESLAB_HPP
