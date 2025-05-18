#ifndef CHARGEDPARTICLE_HPP
#define CHARGEDPARTICLE_HPP
#include "particle.hpp"
#include "basematerial.hpp"

class ChargedParticle : public Particle {
public:
    ChargedParticle(double x, double y, double z, double vx, double vy, double vz, double charge, double mass);

    void propagate(const BaseMaterial& material) override;
    void applyEnergyLoss(const BaseMaterial& material, double stepLength);
    void applyMultipleScattering(const BaseMaterial& material);
    std::array<double, 3> getThermalStep(const BaseMaterial& material);
    void applyDragForce(const BaseMaterial& material);

    bool getAbsorption(const BaseMaterial& material) const override;
    bool isAbsorbed() const;

protected:
    double charge;
    double mass;
    bool is_absorbed;
};

#endif