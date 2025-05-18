#include "particle.hpp"
#include <fstream>
#include <random>
#include <cmath>
#include <utility>


// Save the position in history
void Particle::appendHistory() {
    history.push_back(position);
}

// Save full history in a file
void Particle::saveHistoryToFile(const std::string& filename) const {
    std::ofstream file(filename);
    for (const auto& pos : history) {
        file << pos[0] << " " << pos[1] << " " << pos[2] << "\n";
    }
    file.close();
}

std::pair<double, double> Particle::getRandomSphericalCoordinates() {
    std::random_device rd;  
    std::mt19937 gen(rd()); 

    int min = 0, max = 1;
    std::uniform_real_distribution<> distrib(min, max);

    double phi = distrib(gen) * 2 * M_PI;
    double theta = acos( 2 * distrib(gen) - 1);

    return std::make_pair(phi, theta);
}