#include "sphere.hpp"
#include "particle.hpp"

bool Sphere::isWithinBounds(const  Particle& particle) const {
    double x = particle.getPosition()[0];
    double y = particle.getPosition()[1];
    double z = particle.getPosition()[2];

    double dist = pow( pow(x, 2) + pow(y, 2) + pow(z , 2), 0.5 );
    return dist <= radius;
}
