#include "chargedparticle.hpp"
#include "doubleslab.hpp"
#include <random>
#include <cmath>

ChargedParticle::ChargedParticle(double x, double y, double z,
                                 double vx, double vy, double vz,
                                 double charge_, double mass_)
    : Particle(x, y, z, vx, vy, vz), charge(charge_), mass(mass_), is_absorbed(false)
{}

std::array<double, 3> ChargedParticle::getThermalStep(const BaseMaterial& material) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> distrib(0.0, 1.0);

    double stepLength = -material.getLambda() * log(distrib(gen));

    auto angles = getRandomSphericalCoordinates();
    double phi = angles.first;
    double theta = angles.second;

    double dx = stepLength * sin(theta) * cos(phi);
    double dy = stepLength * sin(theta) * sin(phi);
    double dz = stepLength * cos(theta);

    return {dx, dy, dz};
}

void ChargedParticle::applyEnergyLoss(const BaseMaterial& material, double stepLength) {
    if (is_absorbed) return;

    double dE_dx = material.getStoppingPower();  
    double energyLoss = dE_dx * stepLength * charge * charge / mass;

    double v_mag = sqrt(velocity[0]*velocity[0] + velocity[1]*velocity[1] + velocity[2]*velocity[2]);
    double kineticEnergy = 0.5 * mass * v_mag * v_mag;

    kineticEnergy -= energyLoss;
    
    if (kineticEnergy <= 0) {
        is_absorbed = true;
        for (int i = 0; i < 3; ++i) velocity[i] = 0.0;
        return;
    }

    double new_v_mag = sqrt(2 * kineticEnergy / mass);
    for (int i = 0; i < 3; ++i) {
        velocity[i] *= (new_v_mag / v_mag);
    }
}

void ChargedParticle::applyDragForce(const BaseMaterial& material) {
    double k = material.getK();
    for (int i = 0; i < 3; ++i) {
        velocity[i] *= (1.0 - k);
    }
}

bool ChargedParticle::getAbsorption(const BaseMaterial& material) const {
    if (is_absorbed) return true;

    std::random_device rd;  
    std::mt19937 gen(rd()); 
    std::uniform_real_distribution<> distrib(0.0, 1.0);

    return distrib(gen) < material.getPabs();
}

bool ChargedParticle::isAbsorbed() const {
    return is_absorbed;
}

void ChargedParticle::propagate(const BaseMaterial& material) {  
    
    if (const DoubleSlab* slab = dynamic_cast<const DoubleSlab*>(&material)){
        propagate(*slab);
        return;
    }

    appendHistory();

    std::array<double, 3> thermalStep = getThermalStep(material);

    double stepLength = sqrt(
        thermalStep[0] * thermalStep[0] +
        thermalStep[1] * thermalStep[1] +
        thermalStep[2] * thermalStep[2]
    );

    for (int i = 0; i < 3; ++i) {
        position[i] += thermalStep[i] + velocity[i];  
    }

    applyEnergyLoss(material, stepLength);  

    applyDragForce(material);
    
}

void ChargedParticle::propagate(const DoubleSlab&  doubleSlab) {
    if (doubleSlab.getMaterial1().isWithinBounds(*this)) {
        propagate(doubleSlab.getMaterial1());
    }

    if (doubleSlab.getMaterial2().isWithinBounds(*this)) {
        propagate(doubleSlab.getMaterial2());
    }
}