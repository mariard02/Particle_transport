// particle.hpp
#ifndef PARTICLE_HPP
#define PARTICLE_HPP

#include <vector>
#include <array>
#include <string> 
#include "basematerial.hpp"

class Particle {
protected:
    std::array<double, 3> position;
    std::array<double, 3> velocity;
    std::vector<std::array<double, 3>> history;

public:
    Particle(double x, double y, double z, double vx, double vy, double vz)
        : position({x, y, z}), velocity({vx, vy, vz}) {}

    virtual ~Particle() = default;

    void appendHistory();
    void saveHistoryToFile(const std::string& filename) const;

    std::pair<double, double> getRandomSphericalCoordinates();

    std::array<double, 3> getPosition() const { return position; }
    std::array<double, 3> getVelocity() const { return velocity; }

    virtual void propagate(const BaseMaterial& material) = 0;
    virtual bool getAbsorption(const BaseMaterial& material) const = 0;
};

#endif
