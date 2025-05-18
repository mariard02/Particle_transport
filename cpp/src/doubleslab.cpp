#include "doubleslab.hpp"
#include "particle.hpp"

bool DoubleSlab::isWithinBounds(const  Particle& particle) const {
    double x = particle.getPosition()[0];
    return x >= xinit && x <= xinit + totalLength;
}

