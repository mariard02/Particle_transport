#ifndef NEUTRON_HH
#define NEUTRON_HH

#include <vector>
#include <array>
#include <string> 
#include "particle.hpp"
#include "basematerial.hpp"
#include "doubleslab.hpp"

class DoubleSlab;

class Neutron : public Particle {
private:
    std::vector<std::array<double, 3>> history;

public:
    // Constructor
    Neutron(double x, double y, double z, double vx, double vy, double vz);

    virtual ~Neutron() = default;
    
    double getRandomStepLength(const BaseMaterial& material);
    std::array<double, 3> getThermalStep(const BaseMaterial& material);

    void elasticScatter(const BaseMaterial&  material);
    void applyDragForce(const BaseMaterial& material);
    void propagate(const BaseMaterial&  material) override;
    void propagate(const DoubleSlab& doubleSlab);
    bool getAbsorption(const BaseMaterial&  material) const override;
    bool getAbsorption(const DoubleSlab&  material) const;
};

#endif
