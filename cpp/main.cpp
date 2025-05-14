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
#include <vector>
#include <string>
#include "json.hpp"

using json = nlohmann::json;

struct ConfigError {
    std::vector<std::string> errors;
    
    void add_error(const std::string& error) {
        errors.push_back(error);
    }
    
    bool has_errors() const {
        return !errors.empty();
    }
    
    void print_errors() const {
        for (const auto& err : errors) {
            std::cerr << err << std::endl;
        }
    }
};

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

void check_json_field(const json& j, const std::string& path, ConfigError& error) {
    if (j.is_null()) {
        error.add_error("Error: Required configuration value '" + path + "' is null or missing");
    }
}

void validate_config(const json& config, ConfigError& error) {
    check_json_field(config["run"], "run", error);
    check_json_field(config["run"]["simulations"], "run.simulations", error);
    check_json_field(config["run"]["run_name"], "run.run_name", error);
    check_json_field(config["geometry"], "geometry", error);
    check_json_field(config["geometry"]["shape"], "geometry.shape", error);
    check_json_field(config["particle"], "particle", error);
    check_json_field(config["particle"]["x"], "particle.x", error);
    check_json_field(config["particle"]["y"], "particle.y", error);
    check_json_field(config["particle"]["z"], "particle.z", error);
    check_json_field(config["particle"]["vx"], "particle.vx", error);
    check_json_field(config["particle"]["vy"], "particle.vy", error);
    check_json_field(config["particle"]["vz"], "particle.vz", error);

    if (!config["geometry"]["shape"].is_null()) {
        std::string shape = config["geometry"]["shape"];
        
        if (shape == "regular_slab") {
            check_json_field(config["material"], "material", error);
            check_json_field(config["material"]["mean_free_path"], "material.mean_free_path", error);
            check_json_field(config["material"]["pabs"], "material.pabs", error);
            check_json_field(config["material"]["k"], "material.k", error);
            check_json_field(config["geometry"]["x_init"], "geometry.x_init", error);
        }
        else if (shape == "sphere") {
            check_json_field(config["material"], "material", error);
            check_json_field(config["material"]["mean_free_path"], "material.mean_free_path", error);
            check_json_field(config["material"]["pabs"], "material.pabs", error);
            check_json_field(config["material"]["k"], "material.k", error);
        }
        else if (shape == "finite_slab") {
            check_json_field(config["material"], "material", error);
            check_json_field(config["material"]["mean_free_path"], "material.mean_free_path", error);
            check_json_field(config["material"]["pabs"], "material.pabs", error);
            check_json_field(config["material"]["k"], "material.k", error);
            check_json_field(config["geometry"]["x_length"], "geometry.x_length", error);
            check_json_field(config["geometry"]["y_length"], "geometry.y_length", error);
        }
        else if (shape == "double_slab") {
            check_json_field(config["material"], "material", error);
            check_json_field(config["material"]["mean_free_path_1"], "material.mean_free_path_1", error);
            check_json_field(config["material"]["pabs_1"], "material.pabs_1", error);
            check_json_field(config["material"]["k1"], "material.k1", error);
            check_json_field(config["material"]["mean_free_path_2"], "material.mean_free_path_2", error);
            check_json_field(config["material"]["pabs_2"], "material.pabs_2", error);
            check_json_field(config["material"]["k2"], "material.k2", error);
            check_json_field(config["geometry"]["total_length"], "geometry.total_length", error);
            check_json_field(config["geometry"]["x_init"], "geometry.x_init", error);
        }
        else {
            error.add_error("ERROR. Geometry not supported: " + shape);
        }
    }
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
    try {
        config_file >> config;
    } catch (const json::parse_error& e) {
        std::cerr << "Error parsing JSON file: " << e.what() << "\n";
        return 1;
    }

    ConfigError config_error;
    validate_config(config, config_error);
    
    if (config_error.has_errors()) {
        config_error.print_errors();
        return 1;
    }

    int NumberSims = config["run"]["simulations"];
    std::string run_name = config["run"]["run_name"];
    std::string shape = config["geometry"]["shape"];
    double length = std::atof(argv[2]);
    bool save_histories = config["run"].contains("save_hist"); 

    std::__fs::filesystem::create_directories("../out/" + run_name + "/data");

    std::vector<double> absorbed_ratios, reflected_ratios, transmitted_ratios;
    bool saved_absorbed = false, saved_reflected = false, saved_transmitted = false;

    double x0 = config["particle"]["x"];
    double y0 = config["particle"]["y"];
    double z0 = config["particle"]["z"];
    double vx = config["particle"]["vx"];
    double vy = config["particle"]["vy"];
    double vz = config["particle"]["vz"];

    std::unique_ptr<BaseMaterial> material;
    
    try {
        if (shape == "regular_slab") {
            if (config["material"]["A"].is_null()){
                material = std::make_unique<RegularSlab>(
                    config["material"]["mean_free_path"],
                    config["material"]["pabs"],
                    config["material"]["k"],
                    length,
                    config["geometry"]["x_init"]
                );
            } else {
                material = std::make_unique<RegularSlab>(
                    config["material"]["mean_free_path"],
                    config["material"]["pabs"],
                    config["material"]["k"],
                    length,
                    config["geometry"]["x_init"],
                    config["material"]["A"]
                );
            }

        }
        else if (shape == "sphere") {
            if (config["material"]["A"].is_null()) {
                material = std::make_unique<Sphere>(
                config["material"]["mean_free_path"],
                config["material"]["pabs"],
                config["material"]["k"],
                length
            );
            } else {
                material = std::make_unique<Sphere>(
                config["material"]["mean_free_path"],
                config["material"]["pabs"],
                config["material"]["k"],
                length,
                config["material"]["A"]
            );
            }

        }
        else if (shape == "finite_slab") {
            if (config["material"]["A"].is_null()) {
                material = std::make_unique<FiniteSlab>(
                config["material"]["mean_free_path"],
                config["material"]["pabs"],
                config["material"]["k"],
                config["geometry"]["x_length"],
                config["geometry"]["y_length"],
                length
            );
            } else {
                material = std::make_unique<FiniteSlab>(
                config["material"]["mean_free_path"],
                config["material"]["pabs"],
                config["material"]["k"],
                config["geometry"]["x_length"],
                config["geometry"]["y_length"],
                length,
                config["material"]["A"]
            );
            }

        }
        else if (shape == "double_slab") {
            material = std::make_unique<DoubleSlab>(
                config["material"]["mean_free_path1"],
                config["material"]["pabs1"],
                config["material"]["k1"],
                config["material"]["mean_free_path2"],
                config["material"]["pabs2"],
                config["material"]["k2"],
                config["geometry"]["total_length"],
                config["geometry"]["x_init"],
                length
            );
        }
    } catch (const std::exception& e) {
        std::cerr << "Error creating material: " << e.what() << std::endl;
        return 1;
    }

    for (int run = 0; run < 10; run++) {
        int NumAbsorbed = 0, NumReflected = 0, NumTransmitted = 0;

        for (int i = 0; i < NumberSims; i++) {
            Neutron n(x0, y0, z0, vx, vy, vz);

            if (!material->isWithinBounds(n)) {
                std::cerr << "ERROR. The neutron starts outside the material." << std::endl;
                return 2;
            }

            bool absorbed = false;
            bool reflected = false;

            n.propagate(*material);

            if (!material->isWithinBounds(n)) {
                reflected = true;
            } else {
                while (material->isWithinBounds(n)) {
                    if (n.getAbsorption(*material)) {
                        absorbed = true;
                        break;
                    }
                    n.propagate(*material);
                }
            }

            if (absorbed) NumAbsorbed++;
            else if (reflected) NumReflected++;
            else NumTransmitted++;

            if (save_histories) {
                if (absorbed && !saved_absorbed) {
                    n.saveHistoryToFile("../out/" + run_name + "/data/hist_absorbed.txt");
                    saved_absorbed = true;
                } 
                else if (reflected && !saved_reflected) {
                    n.saveHistoryToFile("../out/" + run_name + "/data/hist_reflected.txt");
                    saved_reflected = true;
                } 
                else if (!reflected && !absorbed && !saved_transmitted) {
                    n.saveHistoryToFile("../out/" + run_name + "/data/hist_transmitted.txt");
                    saved_transmitted = true;
                }
            }
        }

        absorbed_ratios.push_back(static_cast<double>(NumAbsorbed) / NumberSims);
        reflected_ratios.push_back(static_cast<double>(NumReflected) / NumberSims);
        transmitted_ratios.push_back(static_cast<double>(NumTransmitted) / NumberSims);
    }

    double mean_abs = compute_mean(absorbed_ratios);
    double mean_refl = compute_mean(reflected_ratios);
    double mean_trans = compute_mean(transmitted_ratios);

    std::cout << mean_abs << " " << compute_stddev(absorbed_ratios, mean_abs) << " "
              << mean_refl << " " << compute_stddev(reflected_ratios, mean_refl) << " "
              << mean_trans << " " << compute_stddev(transmitted_ratios, mean_trans) << std::endl;

    return 0;
}