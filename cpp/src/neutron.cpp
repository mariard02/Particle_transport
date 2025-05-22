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

    return - material.getLambda() * log(distrib(gen));
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

void Neutron::elasticScatter(const BaseMaterial&  material) {
    double A = material.getAtomicMass();
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

void Neutron::applyDragForce(const BaseMaterial& material) {
    double k = material.getK();
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

    return distrib(gen) < material.getPabs();
}

bool Neutron::getAbsorption(const DoubleSlab& material) const{
    std::random_device rd;  
    std::mt19937 gen(rd()); 

    int min = 0, max = 1;
    std::uniform_real_distribution<> distrib(min, max);

    if (material.getMaterial1().isWithinBounds(*this)) {
        return distrib(gen) < material.getMaterial1().getPabs();
    } else if (material.getMaterial2().isWithinBounds(*this)) {
        return distrib(gen) < material.getMaterial2().getPabs();
    }

    return false;
}

void Neutron::propagate(const BaseMaterial&  material) {
    
    appendHistory();

    std::array<double, 3> thermalStep = getThermalStep(material);

    if (const DoubleSlab* slab = dynamic_cast<const DoubleSlab*>(&material)) {
        propagate(*slab);
        return;
    }

    for (int i = 0; i < 3; ++i) {
        position[i] += thermalStep[i] + velocity[i];
    }

    if (material.hasElasticScattering()) {
        elasticScatter(material);
    } else {
        applyDragForce(material);
    }
}

void Neutron::propagate(const DoubleSlab& doubleSlab) {
    appendHistory();

    double lambda1 = doubleSlab.getMaterial1().getLambda();
    double lambda2 = doubleSlab.getMaterial2().getLambda();
    double lambda_min = std::min(lambda1, lambda2);

    double step_length = getRandomStepLength(doubleSlab) * lambda_min;

    for (int i = 0; i < 3; ++i) {
        position[i] += velocity[i] * step_length;
    }

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
        propagate(*collision_material);
    }

}