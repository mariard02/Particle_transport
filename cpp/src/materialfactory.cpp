#include "materialfactory.hpp"

void MaterialFactory::validate_config(const json& config, ConfigError& error) {
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
    check_json_field(config["particle"]["type"], "particle.type", error);

    std::string type = config["particle"]["type"];
    if (type == "charged") {
        check_json_field(config["particle"]["charge"], "particle.charge", error);
        check_json_field(config["particle"]["mass"], "particle.mass", error);
        check_json_field(config["material"]["absorption_power"], "material.absorption_power", error);
    }

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
            check_json_field(config["material"]["mean_free_path1"], "material.mean_free_path1", error);
            check_json_field(config["material"]["pabs1"], "material.pabs_1", error);
            check_json_field(config["material"]["k1"], "material.k1", error);
            check_json_field(config["material"]["mean_free_path2"], "material.mean_free_path2", error);
            check_json_field(config["material"]["pabs2"], "material.pabs2", error);
            check_json_field(config["material"]["k2"], "material.k2", error);
            check_json_field(config["geometry"]["total_length"], "geometry.total_length", error);
            check_json_field(config["geometry"]["x_init"], "geometry.x_init", error);
        }
        else {
            error.add_error("ERROR. Geometry not supported: " + shape);
        }
    }
};

