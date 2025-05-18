#include "regularslab.hpp"
#include "particle.hpp"

bool RegularSlab::isWithinBounds(const  Particle& particle) const {
    double x = particle.getPosition()[0];
    return x >= xinit && x <= length + xinit;
}
