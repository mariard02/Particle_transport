#include "finiteslab.hpp"
#include "neutron.hpp"

bool FiniteSlab::isWithinBounds(const  Neutron& particle) const {
    double x = particle.getPosition()[0];
    double y = particle.getPosition()[1];
    double z = particle.getPosition()[2];
    return x >= 0.0 && x <= xlength && y >= 0.0 && y <= ylength && z >= 0.0 && z <= zlength;
}
