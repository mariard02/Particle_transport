#ifndef MATERIALFACTORY_HPP
#define MATERIALFACTORY_HPP
#include "memory"
#include "json.hpp"
#include "basematerial.hpp"
#include "regularslab.hpp"
#include "doubleslab.hpp"
#include "finiteslab.hpp"
#include "sphere.hpp"
#include "configerror.hpp"

using json = nlohmann::json;

class MaterialFactory {
public:
    MaterialFactory() {}
    static std::unique_ptr<BaseMaterial> createMaterial(
        const json& config,
        const std::string& shape,
        double length,
        bool isCharged
    ) {
        if (shape == "regular_slab") {
            return createRegularSlab(config, length, isCharged);
        } else if (shape == "sphere") {
            return createSphere(config, length, isCharged);
        } else if (shape == "finite_slab") {
            return createFiniteSlab(config, length, isCharged);
        } else if (shape == "double_slab") {
            return createDoubleSlab(config, length, isCharged);
        } else {
            throw std::runtime_error("Unsupported geometry: " + shape);
        }
    }

    void check_json_field(const json& j, const std::string& path, ConfigError& error) {
        if (j.is_null()) {
            error.add_error("Error: Required configuration value '" + path + "' is null or missing");
        }
    }

    void validate_config(const json& config, ConfigError& error);

private:
    static std::unique_ptr<BaseMaterial> createRegularSlab(
        const json& config,
        double length,
        bool isCharged
    ) {
        if (config["material"].contains("A") && !config["material"]["A"].is_null()) {
            return std::make_unique<RegularSlab>(
                config["material"]["mean_free_path"],
                config["material"]["pabs"],
                config["material"]["k"],
                length,
                config["geometry"]["x_init"],
                isCharged ? config["material"]["absorption_power"].get<double>() : 0.0,
                config["material"]["A"]
            );
        } else {
            return std::make_unique<RegularSlab>(
                config["material"]["mean_free_path"],
                config["material"]["pabs"],
                config["material"]["k"],
                length,
                config["geometry"]["x_init"],
                isCharged ? config["material"]["absorption_power"].get<double>() : 0.0
            );
        }
    }

    static std::unique_ptr<BaseMaterial> createSphere(
        const json& config,
        double length,
        bool isCharged
    ) {
        if (config["material"].contains("A") && !config["material"]["A"].is_null()) {
            return std::make_unique<Sphere>(
                config["material"]["mean_free_path"],
                config["material"]["pabs"],
                config["material"]["k"],
                length,
                isCharged ? config["material"]["absorption_power"].get<double>() : 0.0,
                config["material"]["A"]
            );
        } else {
            return std::make_unique<Sphere>(
                config["material"]["mean_free_path"],
                config["material"]["pabs"],
                config["material"]["k"],
                length,
                isCharged ? config["material"]["absorption_power"].get<double>() : 0.0
            );
        }
    }

    static std::unique_ptr<BaseMaterial> createFiniteSlab(
        const json& config,
        double length,
        bool isCharged
    ) {
        if (config["material"].contains("A") && !config["material"]["A"].is_null()) {
            return std::make_unique<FiniteSlab>(
                config["material"]["mean_free_path"],
                config["material"]["pabs"],
                config["material"]["k"],
                config["geometry"]["x_length"],
                config["geometry"]["y_length"],
                length,
                isCharged ? config["material"]["absorption_power"].get<double>() : 0.0,
                config["material"]["A"]
            );
        } else {
            return std::make_unique<FiniteSlab>(
                config["material"]["mean_free_path"],
                config["material"]["pabs"],
                config["material"]["k"],
                config["geometry"]["x_length"],
                config["geometry"]["y_length"],
                length,
                isCharged ? config["material"]["absorption_power"].get<double>() : 0.0
            );
        }
    }

    static std::unique_ptr<BaseMaterial> createDoubleSlab(
    const json& config,
    double length,
    bool isCharged
) {
    if (config["material"].contains("A") && !config["material"]["A"].is_null()) {
        return std::make_unique<DoubleSlab>(
            config["material"]["mean_free_path1"],
            config["material"]["pabs1"],
            config["material"]["k1"],
            config["material"]["mean_free_path2"],
            config["material"]["pabs2"],
            config["material"]["k2"],
            config["geometry"]["total_length"],
            config["geometry"]["x_init"],
            length,
            isCharged ? config["material"]["absorption_power1"].get<double>() : 0.0,
            isCharged ? config["material"]["absorption_power2"].get<double>() : 0.0,
            config["material"]["A1"],
            config["material"]["A2"]            
        );
    } else {
        return std::make_unique<DoubleSlab>(
            config["material"]["mean_free_path1"],
            config["material"]["pabs1"],
            config["material"]["k1"],
            config["material"]["mean_free_path2"],
            config["material"]["pabs2"],
            config["material"]["k2"],
            config["geometry"]["total_length"],
            config["geometry"]["x_init"],
            length,
            isCharged ? config["material"]["absorption_power1"].get<double>() : 0.0,
            isCharged ? config["material"]["absorption_power2"].get<double>() : 0.0
        );
    }
}
};

#endif