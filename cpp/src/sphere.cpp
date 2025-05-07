#include "sphere.hpp"
#include "neutron.hpp"

bool Sphere::isWithinBounds(const  Neutron& particle) const {
    double x = particle.getPosition()[0];
    double y = particle.getPosition()[1];
    double z = particle.getPosition()[2];

    double dist = pow( pow(x, 2) + pow(y, 2) + pow(z , 2), 0.5 );
    return dist <= radius;
}
