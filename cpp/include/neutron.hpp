#ifndef NEUTRON_HH
#define NEUTRON_HH

#include <vector>
#include <array>
#include <string> 
#include "basematerial.hpp"

class DoubleSlab;

class Neutron {
    private:
        std::array<double, 3> position;
        std::array<double, 3> velocity;
        std::vector<std::array<double, 3>> history;

    public:
        // Constructor
        Neutron(double x, double y, double z, double vx, double vy, double vz);

        virtual ~Neutron() = default;

        // Get methods
        std::array<double, 3> getPosition() const;
        std::array<double, 3> getVelocity() const;

        // History methods
        void appendHistory();
        void saveHistoryToFile(const std::string& filename) const;

        std::pair<double, double> getRandomSphericalCoordinates();

        double getRandomStepLength(const BaseMaterial& material);

        std::array<double, 3> getThermalStep(const BaseMaterial& material);

        void elasticScatter(const BaseMaterial&  material);

        void propagate(const BaseMaterial&  material);
        void propagate(const DoubleSlab doubleSlab);

        bool getAbsorption(const BaseMaterial&  material);

};

#endif
