#include "finiteslab.hpp"
#include "particle.hpp"

bool FiniteSlab::isWithinBounds(const  Particle& particle) const {
    double x = particle.getPosition()[0];
    double y = particle.getPosition()[1];
    double z = particle.getPosition()[2];
    return x >= - xlength/2 && x <= xlength/2 && y >= - ylength/2 && y <= ylength/2 && z >= 0 && z <= zlength;
}
