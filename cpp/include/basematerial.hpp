#ifndef BASEMATERIAL_HPP
#define BASEMATERIAL_HPP

// Forward declaration of the Particle class
class Particle;

/**
 * @brief Abstract base class representing a material.
 * 
 * This class defines common properties and interfaces for different types of materials
 * that can interact with particles. It is meant to be subclassed by concrete material.
 */
class BaseMaterial {
private:

public:

    BaseMaterial() {};

    // Virtual destructor to ensure proper cleanup in derived classes
    virtual ~BaseMaterial() = default;

    // Accessor methods. They must be implemented in derived classes, depending on whether we have a simple or a composite material.
    virtual double getLambda(const Particle& p) const = 0;
    virtual double getPabs(const Particle& p) const = 0;
    virtual double getK(const Particle& p) const = 0;

    /**
     * @brief Check if a given particle is still within the material boundaries.
     * 
     * This method must be implemented in derived classes depending on geometry.
     * 
     * @param particle Reference to the particle
     * @return true if the particle is inside the material
     */
    virtual bool isWithinBounds(const Particle& particle) const = 0;

    /**
     * @brief Indicates if the material supports elastic scattering (i.e., has defined atomic mass).
     * 
     * @return true if atomic mass > 0
     */
    virtual bool hasElasticScattering(const Particle& p) const = 0;

    virtual double getAtomicMass(const Particle& p) const = 0;

    /**
     * @brief Returns the stopping power (for charged particles).
     */
    virtual double getStoppingPower(const Particle& p) const = 0;
};

#endif // BASEMATERIAL_HPP
