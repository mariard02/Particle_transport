#include "regularslab.hpp"
#include "neutron.hpp"

bool RegularSlab::isWithinBounds(const  Neutron& particle) const {
    double x = particle.getPosition()[0];
    return x >= xinit && x <= length + xinit;
}
