#include "chargedparticle.hpp"
#include "doubleslab.hpp"
#include <random>
#include <cmath>
#include "iostream"

ChargedParticle::ChargedParticle(double x, double y, double z,
                                 double vx, double vy, double vz,
                                 double charge_, double mass_)
    : Particle(x, y, z, vx, vy, vz), charge(charge_), mass(mass_), is_absorbed(false)
{}

double ChargedParticle::getRandomStepLength(const BaseMaterial&  material) {
    std::random_device rd;  
    std::mt19937 gen(rd()); 

    int min = 0, max = 1;
    std::uniform_real_distribution<> distrib(min, max);

    return - material.getLambda(*this) * log(distrib(gen));
}

std::array<double, 3> ChargedParticle::getThermalStep(const BaseMaterial& material) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> distrib(0.0, 1.0);

    auto r = getRandomStepLength(material);

    auto angles = getRandomSphericalCoordinates();
    double phi = angles.first;
    double theta = angles.second;

    double dx = r * sin(theta) * cos(phi);
    double dy = r * sin(theta) * sin(phi);
    double dz = r * cos(theta);

    return {dx, dy, dz};
}

void ChargedParticle::applyEnergyLoss(const BaseMaterial& material, double stepLength) {
    if (is_absorbed) return;

    double dE_dx = material.getStoppingPower(*this);  
    double energyLoss = dE_dx * stepLength;
    

    double v_mag = sqrt(velocity[0]*velocity[0] + velocity[1]*velocity[1] + velocity[2]*velocity[2]);
    double kineticEnergy = 0.5 * mass * v_mag * v_mag;
    kineticEnergy = kineticEnergy - energyLoss;
    
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
    double k = material.getK(*this);
    for (int i = 0; i < 3; ++i) {
        velocity[i] *= (1.0 - k);
    }
}

bool ChargedParticle::getAbsorption(const BaseMaterial& material) const {
    if (is_absorbed) return true;

    std::random_device rd;  
    std::mt19937 gen(rd()); 
    std::uniform_real_distribution<> distrib(0.0, 1.0);

    return distrib(gen) < material.getPabs(*this);
}

bool ChargedParticle::isAbsorbed() const {
    return is_absorbed;
}

void ChargedParticle::applyMultipleScattering(const BaseMaterial& material){
    double A = material.getAtomicMass(*this);
    if (A <= 0) return;

    auto angles = getRandomSphericalCoordinates();

    float phi = angles.first;
    float theta = angles.second;

    double ux = sin(theta) * cos(phi);
    double uy = sin(theta) * sin(phi);
    double uz = cos(theta);

    double v_initial = sqrt(
        velocity[0]*velocity[0] +
        velocity[1]*velocity[1] +
        velocity[2]*velocity[2]
    );

    double scale = sqrt((A*A + 1 + 2*A*cos(theta)) / pow(A + 1, 2));
    double v_final = v_initial * scale;

    velocity = {v_final * ux, v_final * uy, v_final * uz};
}

void ChargedParticle::propagate(const BaseMaterial& material) {  
    std::array<double, 3> thermalStep = getThermalStep(material);

    double stepLength = sqrt( thermalStep[0] * thermalStep [0] +      
        thermalStep[1] * thermalStep [1] + 
        thermalStep[2] * thermalStep [2] ); // Not possible to use getTermalStepLenght, it would give a different value.

    if (const DoubleSlab* slab = dynamic_cast<const DoubleSlab*>(&material)) {
        propagate(*slab);
        return;
    }

    for (int i = 0; i < 3; ++i) {
        position[i] += thermalStep[i] + velocity[i];
    }

    if (material.hasElasticScattering(*this)) {
        applyMultipleScattering(material);
    } else {
        applyDragForce(material);
    }
    applyEnergyLoss(material, stepLength);  

    appendHistory();
    
}

void ChargedParticle::propagate(const DoubleSlab&  doubleSlab) {
    double lambda1 = doubleSlab.getMaterial1().getLambda(*this);
    double lambda2 = doubleSlab.getMaterial2().getLambda(*this);
    double lambda_min = std::min(lambda1, lambda2);

    double step_length = getRandomStepLength(doubleSlab)/doubleSlab.getLambda(*this) * lambda_min; 

    bool in_mat1 = doubleSlab.getMaterial1().isWithinBounds(*this);
    bool in_mat2 = doubleSlab.getMaterial2().isWithinBounds(*this);

    if (!in_mat1 && !in_mat2) return;

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> distrib(0, 1);

    double P_collision = 0.0;
    const RegularSlab* collision_material = nullptr;

    if (in_mat1 && in_mat2) {
        P_collision = (lambda_min/lambda1 + lambda_min/lambda2) / 2.0;
        collision_material = (distrib(gen) < lambda2/(lambda1 + lambda2)) ? 
                           &doubleSlab.getMaterial1() : &doubleSlab.getMaterial2();
    } 
    else if (in_mat1) {
        P_collision = lambda_min / lambda1;
        collision_material = &doubleSlab.getMaterial1();
    } 
    else {
        P_collision = lambda_min / lambda2;
        collision_material = &doubleSlab.getMaterial2();
    }

    if (distrib(gen) < P_collision && collision_material) {
        propagate(*collision_material); // This is what changes the velocity direction and modulus. If it does not enter here, the velocity does not change
    } else {
        for (int i = 0; i < 3; ++i) {
            position[i] += step_length; // Just apply the non thermal velocity to the accepted steps. Otherwise, it is applied more often than it should
        }
    }
}