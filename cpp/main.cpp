#include "neutron.hpp"
#include "chargedparticle.hpp"
#include "basematerial.hpp"
#include "materialfactory.hpp"
#include <iostream>
#include <fstream>
#include <random>
#include <cmath>
#include <vector>
#include <string>

// Computes the mean of a vector of doubles
double compute_mean(const std::vector<double>& values) {
    double sum = 0.0;
    for (double v : values) sum += v;
    return sum / values.size();
}

// Computes the standard deviation of a vector of doubles, given the mean
double compute_stddev(const std::vector<double>& values, double mean) {
    double sum = 0.0;
    for (double v : values) sum += (v - mean) * (v - mean);
    return std::sqrt(sum / values.size());
}

int main(int argc, char* argv[]) {
    // Ensure correct number of command-line arguments
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <config_file.json> <scale>\n";
        return 1;
    }

    // Open the configuration file
    std::ifstream config_file(argv[1]);
    if (!config_file) {
        std::cerr << "Error: Could not open config file.\n";
        return 1;
    }

    json config;
    try {
        config_file >> config;  // Parse JSON
    } catch (const json::parse_error& e) {
        std::cerr << "Error parsing JSON file: " << e.what() << "\n";
        return 1;
    }

    // Validate configuration
    ConfigError config_error;
    MaterialFactory configuration;
    configuration.validate_config(config, config_error);
    
    if (config_error.has_errors()) {
        config_error.print_errors();  // Print any validation errors
        return 1;
    }

    // Read simulation settings from the configuration
    int NumberSims = config["run"]["simulations"];
    std::string run_name = config["run"]["run_name"];
    std::string shape = config["geometry"]["shape"];
    double length = std::atof(argv[2]);  // Scale factor passed via command line
    bool save_histories = config["run"].contains("save_hist");  // Optional history saving

    // Create output directory
    std::__fs::filesystem::create_directories("../out/" + run_name + "/data");

    // Vectors to accumulate outcome ratios across multiple runs
    std::vector<double> absorbed_ratios, reflected_ratios, scaped_ratios;
    bool saved_absorbed = false, saved_reflected = false, saved_scaped = false;

    // Initial particle conditions
    double x0 = config["particle"]["x"];
    double y0 = config["particle"]["y"];
    double z0 = config["particle"]["z"];
    double vx = config["particle"]["vx"];
    double vy = config["particle"]["vy"];
    double vz = config["particle"]["vz"];
    std::string particle_type = config["particle"]["type"];

    // Charge and mass are only relevant for charged particles
    double charge = 0.0;
    double mass = 0.0;
    if (particle_type == "charged") {
        charge = config["particle"]["charge"];
        mass = config["particle"]["mass"];
    }

    // Create material object based on configuration
    std::unique_ptr<BaseMaterial> material;
    try {
        bool isCharged = (particle_type == "charged");
        material = configuration.createMaterial(config, shape, length, isCharged);
    } catch (const std::exception& e) {
        std::cerr << "Error creating material: " << e.what() << std::endl;
        return 1;
    }

    // Run the simulation multiple times to get statistics
    for (int run = 0; run < 10; run++) {
        int NumAbsorbed = 0, NumReflected = 0, NumScaped = 0;

        for (int i = 0; i < NumberSims; i++) {
            // Create a new particle based on type
            std::unique_ptr<Particle> particle;
            if (particle_type == "neutron") {
                particle = std::make_unique<Neutron>(x0, y0, z0, vx, vy, vz);
            } else if (particle_type == "charged") {
                particle = std::make_unique<ChargedParticle>(x0, y0, z0, vx, vy, vz, charge, mass);
            } else {
                std::cerr << "ERROR: Unknown particle type '" << particle_type << "'\n";
                return 1;
            }

            // Ensure the particle starts within bounds
            if (!material->isWithinBounds(*particle)) {
                std::cerr << "ERROR. The particle starts outside the material." << std::endl;
                return 2;
            }

            bool absorbed = false;
            bool reflected = false;

            double xinit = 0.0;
            // For slab geometries, get the initial x-boundary for later reflection check
            if (shape == "regular_slab" || shape == "double_slab") {
                if (const RegularSlab* slabPtr = dynamic_cast<const RegularSlab*>(material.get())) {
                    xinit = slabPtr->getXInit();
                }
            } else if (shape == "double_slab") { 
                if (const DoubleSlab* slabPtr = dynamic_cast<const DoubleSlab*>(material.get())) {
                    xinit = slabPtr->getXInit();
                }
            }

            // First propagation before checking absorption
            particle->propagate(*material);

            // Particle loop: propagate until out of bounds or absorbed
            while (material->isWithinBounds(*particle)) {
                if (particle->getAbsorption(*material)) {
                    absorbed = true;
                    break;
                }
                particle->propagate(*material);
            }

            particle->appendHistory();

            // Check if the particle was reflected (escaped through the entry side)
            if (!absorbed && (shape == "regular_slab" || shape == "double_slab")) {
                double x = particle->getPosition()[0];
                if (x < xinit) {
                    reflected = true;
                }
            }

            // Record final state of the particle
            if (absorbed) NumAbsorbed++;
            else if (reflected) NumReflected++;
            else NumScaped++;

            // Optionally save particle history if required
            if (save_histories) {
                if (absorbed && !saved_absorbed) {
                    particle->saveHistoryToFile("../out/" + run_name + "/data/hist_absorbed.txt");
                    saved_absorbed = true;
                } 
                else if (reflected && !saved_reflected) {
                    particle->saveHistoryToFile("../out/" + run_name + "/data/hist_reflected.txt");
                    saved_reflected = true;
                } 
                else if (!reflected && !absorbed && !saved_scaped) {
                    particle->saveHistoryToFile("../out/" + run_name + "/data/hist_scaped.txt");
                    saved_scaped = true;
                }
            }
        }

        // Store results from this run
        absorbed_ratios.push_back(static_cast<double>(NumAbsorbed) / NumberSims);
        reflected_ratios.push_back(static_cast<double>(NumReflected) / NumberSims);
        scaped_ratios.push_back(static_cast<double>(NumScaped) / NumberSims);
    }

    // Compute final statistics: mean and standard deviation for each outcome
    double mean_abs = compute_mean(absorbed_ratios);
    double stddev_abs = compute_stddev(absorbed_ratios, mean_abs);
    double mean_ref = compute_mean(reflected_ratios);
    double stddev_ref = compute_stddev(reflected_ratios, mean_ref);
    double mean_sc = compute_mean(scaped_ratios);
    double stddev_sc = compute_stddev(scaped_ratios, mean_sc);

    // Output the statistics
    std::cout << mean_abs << " " << stddev_abs << " "
              << mean_ref << " " << stddev_ref << " "
              << mean_sc << " " << stddev_sc << std::endl;

    return 0;
}
