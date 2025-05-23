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
protected:
    double lambda;         ///< Mean free path of the particle in the material
    double pabs;           ///< Probability of absorption upon interaction
    double k;              ///< Diffusion coefficient or parameter for Brownian motion
    double atomicMass;     ///< Atomic mass of the material
    double stoppingPower;  ///< Energy loss per unit length (relevant for charged particles)

public:
    /**
     * @brief Main constructor.
     * 
     * @param lambda Mean free path
     * @param pabs Absorption probability
     * @param k Diffusion coefficient
     * @param atomicMass Atomic mass (default -1.0 if not applicable)
     * @param stoppingPower Energy loss per unit length (default 0.0)
     */
    BaseMaterial(double lambda, double pabs, double k, double atomicMass = -1.0, double stoppingPower = 0.0)
        : lambda(lambda), pabs(pabs), k(k), atomicMass(atomicMass), stoppingPower(stoppingPower) {}

    /**
     * @brief Default constructor.
     * 
     * Sets all parameters to neutral values. Used for debugging or fallback cases.
     */
    BaseMaterial() : lambda(0.0), pabs(0.0), k(0.0), atomicMass(-1.0), stoppingPower(0.0) {}

    // Virtual destructor to ensure proper cleanup in derived classes
    virtual ~BaseMaterial() = default;

    // Accessor methods
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
