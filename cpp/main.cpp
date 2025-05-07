#include "neutron.hpp"
#include "basematerial.hpp"
#include "regularslab.hpp"
#include "sphere.hpp"
#include "finiteslab.hpp"
#include "doubleslab.hpp"
#include <iostream>
#include <fstream>
#include <random>
#include <cmath>
#include "json.hpp"

using json = nlohmann::json;

double compute_mean(const std::vector<double>& values) {
    double sum = 0.0;
    for (double v : values) sum += v;
    return sum / values.size();
}

double compute_stddev(const std::vector<double>& values, double mean) {
    double sum = 0.0;
    for (double v : values) sum += (v - mean) * (v - mean);
    return std::sqrt(sum / values.size());
}

int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <config_file.json> <scale>\n";
        return 1;
    }

    std::ifstream config_file(argv[1]);
    if (!config_file) {
        std::cerr << "Error: Could not open config file.\n";
        return 1;
    }

    json config;
    config_file >> config;

    int NumberSims = config["run"]["simulations"];
    std::string run_name = config["run"]["run_name"];
    std::string shape = config["geometry"]["shape"];

    std::__fs::filesystem::create_directories("../out/" + run_name + "/data");

    double length = std::atof(argv[2]);

    RegularSlab mat(0.0, 0.0, 0.0, 0.0, 0.0);

    if (shape == "regular_slab"){

        double lambda = config["material"]["mean_free_path"];
        double pabs = config["material"]["pabs"];
        double k = config["material"]["k"];
        double x_init = config["geometry"]["x_init"];

        RegularSlab mat(lambda, pabs, k, length, x_init);

    } else if (shape == "sphere"){

        double lambda = config["material"]["mean_free_path"];
        double pabs = config["material"]["pabs"];
        double k = config["material"]["k"];

        Sphere mat(lambda, pabs, k, length);

    } else if (shape == "finite_slab"){

        double lambda = config["material"]["mean_free_path"];
        double pabs = config["material"]["pabs"];
        double k = config["material"]["k"];

        double xlength = config["geometry"]["x_length"];
        double ylength = config["geometry"]["y_length"];

        FiniteSlab mat(lambda, pabs, k, xlength, ylength, length);

    } else if (shape == "double_slab") {

        double lambda1 = config["material"]["mean_free_path_1"];
        double pabs1 = config["material"]["pabs_1"];
        double k1 = config["material"]["k1"];
        double lambda2 = config["material"]["mean_free_path_2"];
        double pabs2 = config["material"]["pabs_2"];
        double k2 = config["material"]["k2"];

        double totalLength = config["geometry"]["total_length"];
        double xinit = config["geometry"]["x_init"];

        DoubleSlab mat(lambda1, pabs1, k1, lambda2, pabs2, k2, totalLength, xinit, length);

    } else {
        return 1;
    }

    std::vector<double> absorbed_ratios, reflected_ratios, transmitted_ratios;

    bool saved_absorbed = false;
    bool saved_reflected = false;
    bool saved_transmitted = false;

    for (int run = 0; run < 10; run++) {
        
        int NumAbsorbed = 0;
        int NumReflected = 0;
        int NumTransmitted = 0;

        double x0 = config["particle"]["x"];
        double y0 = config["particle"]["y"];
        double z0 = config["particle"]["z"];
        double vx = config["particle"]["vx"];
        double vy = config["particle"]["vy"];
        double vz = config["particle"]["vz"];

        for (int i = 0; i < NumberSims; i++) {
            Neutron n(x0, y0, z0, vx, vy, vz);
            bool absorbed = false;
            bool reflected = false;

            n.propagate(mat);

            if (!mat.isWithinBounds(n)) {
                reflected = true;
            } else {
                while (mat.isWithinBounds(n)) {
                    if (n.getAbsorption(mat)) {
                        absorbed = true;
                        break;
                    }
                    n.propagate(mat);
                }
            }

            if (absorbed) {
                NumAbsorbed++;
            } else if (reflected) {
                NumReflected++;
            } else {
                NumTransmitted++;
            }

            if (absorbed && !saved_absorbed) {
                n.saveHistoryToFile("../out/" + run_name + "/data/hist_absorbed.txt");
                saved_absorbed = true;
            } else if (reflected && !saved_reflected) {
                n.saveHistoryToFile("../out/" + run_name + "/data/hist_reflected.txt");
                saved_reflected = true;
            } else if (!reflected && !absorbed && !saved_transmitted) {
                n.saveHistoryToFile("../out/" + run_name + "/data/hist_transmitted.txt");
                saved_transmitted = true;
            }
        }

        absorbed_ratios.push_back(static_cast<double>(NumAbsorbed) / NumberSims);
        reflected_ratios.push_back(static_cast<double>(NumReflected) / NumberSims);
        transmitted_ratios.push_back(static_cast<double>(NumTransmitted) / NumberSims);
    }

    double mean_abs = compute_mean(absorbed_ratios);
    double std_abs = compute_stddev(absorbed_ratios, mean_abs);

    double mean_refl = compute_mean(reflected_ratios);
    double std_refl = compute_stddev(reflected_ratios, mean_refl);

    double mean_trans = compute_mean(transmitted_ratios);
    double std_trans = compute_stddev(transmitted_ratios, mean_trans);

    std::cout <<  mean_abs << " " << std_abs << " "
              << mean_refl << " " << std_refl << " "
              << mean_trans << " " << std_trans << std::endl;

    return 0;
}
