#include "neutron.hpp"
#include "basematerial.hpp"
#include "regularslab.hpp"
#include "sphere.hpp"
#include <iostream>
#include <fstream>
#include <random>

int main(int argc, char* argv[]) {
    if (argc != 5) {
        std::cerr << "Usage: " << argv[0] << " <lambda> <pabs> <k> <length>\n";
        return 1;
    }

    double lambda = std::atof(argv[1]);
    double pabs = std::atof(argv[2]);
    double k = std::atof(argv[3]);
    double length = std::atof(argv[4]);

    RegularSlab mat(lambda, pabs, k, length);

    int NumberSims = 1000;

    bool saved_absorbed = false;
    bool saved_reflected = false;
    bool saved_transmitted = false;

    int NumAbsorbed = 0;
    int NumReflected = 0;
    int NumTransmitted = 0;

    for (int i = 0; i < NumberSims; i++) {
        Neutron n(0.0, 0.0, 0.0, 0.5, 0.0, 0.0);
        bool absorbed = false;
        bool entered = false;

        while (mat.isWithinBounds(n)) {
            entered = true;
            if (n.getAbsorption(mat)) {
                absorbed = true;
                break;
            }
            n.propagate(mat);
        }

        if (absorbed) {
            NumAbsorbed++;
        } else if (!entered) {
            NumReflected++;
        } else {
            NumTransmitted++;
        }

        if (absorbed && !saved_absorbed) {
            n.saveHistoryToFile("../data/hist_absorbed.txt");
            saved_absorbed = true;
        } else if (!entered && !saved_reflected) {
            n.saveHistoryToFile("../data/hist_reflected.txt");
            saved_reflected = true;
        } else if (entered && !absorbed && !saved_transmitted) {
            n.saveHistoryToFile("../data/hist_transmitted.txt");
            saved_transmitted = true;
        }
    }

    std::cout << static_cast<double>(NumAbsorbed) / NumberSims
              << " " << static_cast<double>(NumReflected) / NumberSims
              << " " << static_cast<double>(NumTransmitted) / NumberSims
              << std::endl;

    return 0;
}
