#include "doubleslab.hpp"
#include "neutron.hpp"

bool DoubleSlab::isWithinBounds(const  Neutron& particle) const {
    double x = particle.getPosition()[0];
    return x >= 0.0 && x <= totalLength;
}

