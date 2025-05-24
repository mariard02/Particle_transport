#ifndef CHARGEDPARTICLE_HPP
#define CHARGEDPARTICLE_HPP

#include "particle.hpp"
#include "basematerial.hpp"
#include "simplematerial.hpp"
#include "doubleslab.hpp"

/**
 * @brief Represents a charged particle that can propagate through a material.
 * 
 * This class extends the base Particle class by including charge, mass, and specific
 * physical interactions such as energy loss, multiple scattering, and drag forces.
 */
class ChargedParticle : public Particle {
public:
    /**
     * @brief Constructor for a charged particle.
     * 
     * @param x Initial x position
     * @param y Initial y position
     * @param z Initial z position
     * @param vx Initial velocity component in x
     * @param vy Initial velocity component in y
     * @param vz Initial velocity component in z
     * @param charge Electric charge of the particle
     * @param mass Mass of the particle
     */
    ChargedParticle(double x, double y, double z, double vx, double vy, double vz, double charge, double mass);

    /**
     * @brief Propagate the particle through a material.
     * 
     * Implements step-by-step propagation, applying energy loss, scattering, etc.
     * 
     * @param material Reference to the material the particle is moving through
     */
    void propagate(const BaseMaterial& material) override;

    /**
     * @brief Apply continuous energy loss according to the material’s stopping power.
     * 
     * @param material Reference to the material
     * @param stepLength Distance traveled in the current step
     */
    void applyEnergyLoss(const BaseMaterial& material, double stepLength);

    double getRandomStepLength(const BaseMaterial&  material);

    /**
     * @brief Apply a random scattering to simulate multiple Coulomb scattering.
     * 
     * This modifies the particle’s direction due to interaction with nuclei/electrons.
     * 
     * @param material Reference to the material
     */
    void elasticScatter(const BaseMaterial& material);

    /**
     * @brief Compute a thermal (Brownian-like) step based on the material’s diffusion coefficient.
     * 
     * @param material Reference to the material
     * @return A 3D displacement vector due to thermal motion
     */
    std::array<double, 3> getThermalStep(const BaseMaterial& material);

    /**
     * @brief Apply a drag force to reduce the particle's velocity over time.
     * 
     * Useful for simulating motion in a medium with friction-like resistance.
     * 
     * @param material Reference to the material
     */
    void applyDragForce(const BaseMaterial& material);

    /**
     * @brief Determines whether the particle is absorbed in the material.
     * 
     * @param material Reference to the material
     * @return true if absorbed based on a random draw and material properties
     */
    bool getAbsorption(const BaseMaterial& material) const override;

    /**
     * @brief Check if the particle has been absorbed (after propagation).
     * 
     * @return true if absorbed
     */
    bool isAbsorbed() const;

    /**
     * @brief Propagate the particle through a composite material (e.g., double slab).
     * 
     * @param doubleSlab Composite geometry of two slabs
     */
    void propagate(const DoubleSlab& doubleSlab);

private:
    double charge;        ///< Electric charge of the particle
    double mass;          ///< Mass of the particle
    bool is_absorbed;     ///< Internal flag indicating whether the particle is absorbed
};

#endif // CHARGEDPARTICLE_HPP
