#include "sphere.hpp"
#include "neutron.hpp"

bool Sphere::isWithinBounds(const  Neutron& particle) const {
    double x = particle.getPosition()[0];
    double y = particle.getPosition()[1];
    double z = particle.getPosition()[2];

    double dist = pow( pow(x - center[0], 2) + pow(y - center[1], 2) + pow(z - center[2], 2), 0.5 );
    return dist <= radius;
}
