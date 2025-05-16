#ifndef PARTICLE_HPP
#define PARTICLE_HPP

#include <array>

class Particle{
    protected:
        std::array<double, 3> position;
        std::array<double, 3> velocity;
        std::vector<std::array<double, 3>> history;

    public:
    
};

#endif