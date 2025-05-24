#include "neutron.hpp"
#include "doubleslab.hpp"
#include <fstream>
#include <random>
#include <cmath>
#include <utility>


Neutron::Neutron(double x, double y, double z, double vx, double vy, double vz)
    : Particle(x, y, z, vx, vy, vz) 
{
    appendHistory();
}

double Neutron::getRandomStepLength(const BaseMaterial&  material) {
    std::random_device rd;  
    std::mt19937 gen(rd()); 

    int min = 0, max = 1;
    std::uniform_real_distribution<> distrib(min, max);

    return - material.getLambda(*this) * log(distrib(gen));
}

std::array<double, 3> Neutron::getThermalStep(const BaseMaterial&  material) {
    auto InitialCoords = Neutron::getPosition();

    auto r = getRandomStepLength(material);
    auto angles = getRandomSphericalCoordinates();

    float phi = angles.first;
    float theta  = angles.second;

    float dx = r * cos(phi) * sin(theta);
    float dy = r * sin(phi) * sin(theta);
    float dz = r * cos(theta);

    std::array<double, 3> DeltaPos = {dx, dy, dz};

    return DeltaPos;

}

void Neutron::elasticScatter(const BaseMaterial& material) {
    double A = material.getAtomicMass(*this);
    if (A <= 0) return;

    double vx = velocity[0], vy = velocity[1], vz = velocity[2];
    double v_initial = std::sqrt(vx*vx + vy*vy + vz*vz);
    if (v_initial == 0.0) return;  

    double ux0 = vx / v_initial;
    double uy0 = vy / v_initial;
    double uz0 = vz / v_initial;

    double v_cm_x = vx / (1.0 + A);
    double v_cm_y = vy / (1.0 + A);
    double v_cm_z = vz / (1.0 + A);

    double v_rel_x = vx - v_cm_x;
    double v_rel_y = vy - v_cm_y;
    double v_rel_z = vz - v_cm_z;

    double v_rel = std::sqrt(v_rel_x*v_rel_x + v_rel_y*v_rel_y + v_rel_z*v_rel_z);

    auto angles = getRandomSphericalCoordinates(); 
    double phi = angles.first;
    double theta = angles.second;

    double ux = std::sin(theta) * std::cos(phi);
    double uy = std::sin(theta) * std::sin(phi);
    double uz = std::cos(theta);

    double v_rel_x_new = v_rel * ux;
    double v_rel_y_new = v_rel * uy;
    double v_rel_z_new = v_rel * uz;

    double v_final_x = v_rel_x_new + v_cm_x;
    double v_final_y = v_rel_y_new + v_cm_y;
    double v_final_z = v_rel_z_new + v_cm_z;

    velocity = {v_final_x, v_final_y, v_final_z};
}


void Neutron::applyDragForce(const BaseMaterial& material) {
    double k = material.getK(*this);
    for (int i = 0; i < 3; ++i) {
        velocity[i] *= (1.0 - k);
    }
}

bool Neutron::getAbsorption(const BaseMaterial&  material) const{
    std::random_device rd;  
    std::mt19937 gen(rd()); 

    int min = 0, max = 1;
    std::uniform_real_distribution<> distrib(min, max);

    if (const DoubleSlab* slab = dynamic_cast<const DoubleSlab*>(&material)) {
       return getAbsorption(*slab);
    }

    return distrib(gen) < material.getPabs(*this);
}

bool Neutron::getAbsorption(const DoubleSlab& material) const{
    std::random_device rd;  
    std::mt19937 gen(rd()); 

    int min = 0, max = 1;
    std::uniform_real_distribution<> distrib(min, max);

    if (material.getMaterial1().isWithinBounds(*this)) {
        return distrib(gen) < material.getMaterial1().getPabs(*this);
    } else if (material.getMaterial2().isWithinBounds(*this)) {
        return distrib(gen) < material.getMaterial2().getPabs(*this);
    }

    return false;
}

void Neutron::propagate(const BaseMaterial&  material) {

    std::array<double, 3> thermalStep = getThermalStep(material);

    if (const DoubleSlab* slab = dynamic_cast<const DoubleSlab*>(&material)) {
        propagate(*slab);
        return;
    }
        
    for (int i = 0; i < 3; ++i) {
        position[i] += thermalStep[i] + velocity[i];
    }

    if (material.hasElasticScattering(*this)) {
        elasticScatter(material);
    } else {
        applyDragForce(material);
    }

    appendHistory();
}

void Neutron::propagate(const DoubleSlab& doubleSlab) {

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