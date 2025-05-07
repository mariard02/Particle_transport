#include "neutron.hpp"
#include "doubleslab.hpp"
#include <fstream>
#include <random>
#include <cmath>
#include <utility>

// Constructor
Neutron::Neutron(double x, double y, double z, double vx, double vy, double vz) {
    position = {x, y, z};
    velocity = {vx, vy, vz};
    appendHistory();  
}

// Get methods
std::array<double, 3> Neutron::getPosition() const {
    return position;
}

std::array<double, 3> Neutron::getVelocity() const {
    return velocity;
}

// Save the position in history
void Neutron::appendHistory() {
    history.push_back(position);
}

// Save full history in a file
void Neutron::saveHistoryToFile(const std::string& filename) const {
    std::ofstream file(filename);
    for (const auto& pos : history) {
        file << pos[0] << " " << pos[1] << " " << pos[2] << "\n";
    }
    file.close();
}

std::pair<double, double> Neutron::getRandomSphericalCoordinates() {
    std::random_device rd;  
    std::mt19937 gen(rd()); 

    int min = 0, max = 1;
    std::uniform_real_distribution<> distrib(min, max);

    double phi = distrib(gen) * 2 * M_PI;
    double theta = acos( 2 * distrib(gen) - 1);

    return std::make_pair(phi, theta);
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

    std::array<double, 3> DeltaPos = {dx, dy, dx};

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

bool Neutron::getAbsorption(const BaseMaterial&  material){
    std::random_device rd;  
    std::mt19937 gen(rd()); 

    int min = 0, max = 1;
    std::uniform_real_distribution<> distrib(min, max);

    double p = distrib(gen);

    return distrib(gen) < material.getPabs();
}

void Neutron::propagate(const BaseMaterial&  material) {
    std::array<double, 3> thermalStep = getThermalStep(material);

    for (int i = 0; i < 3; ++i) {
        position[i] += thermalStep[i] + velocity[i];
    }

    appendHistory();

    if (material.hasElasticScattering()) {
        elasticScatter(material);
    } else {
        double k = material.getK(); 
        for (int i = 0; i < 3; ++i) {
            velocity[i] *= (1.0 - k);
        }
    }

}

void Neutron::propagate(const DoubleSlab  doubleSlab) {

    while (doubleSlab.getMaterial1().isWithinBounds(*this)) {
        std::array<double, 3> thermalStep = getThermalStep(doubleSlab.getMaterial1());

        for (int i = 0; i < 3; ++i) {
            position[i] += thermalStep[i] + velocity[i];
        }

        appendHistory();

        double k = doubleSlab.getMaterial1().getK();
        for (int i = 0; i < 3; ++i) {
            velocity[i] *= (1.0 - k);
        }

        if (getAbsorption(doubleSlab.getMaterial1())) {
            return;
        }
    }

    while (doubleSlab.getMaterial2().isWithinBounds(*this)) {
        std::array<double, 3> thermalStep = getThermalStep(doubleSlab.getMaterial2());

        for (int i = 0; i < 3; ++i) {
            position[i] += thermalStep[i] + velocity[i];
        }

        appendHistory();

        double k = doubleSlab.getMaterial2().getK();
        for (int i = 0; i < 3; ++i) {
            velocity[i] *= (1.0 - k);
        }

        if (getAbsorption(doubleSlab.getMaterial2())) {
            return;
        }
    }
}



