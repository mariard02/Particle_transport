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

/**
 * @brief Factory class to create material objects based on JSON configuration.
 * 
 * This class uses the Factory design pattern to construct different kinds of
 * materials (e.g., slabs, spheres) from a JSON configuration.
 */
class MaterialFactory {
public:
    /// Default constructor
    MaterialFactory() {}

    /**
     * @brief Creates a material object based on shape, configuration and whether the particle is charged.
     * 
     * @param config JSON object with configuration values.
     * @param shape Geometry type ("regular_slab", "sphere", "finite_slab", "double_slab").
     * @param length Length or radius parameter depending on geometry.
     * @param isCharged Whether the particle is charged (activates stopping power).
     * @return A unique pointer to the created BaseMaterial object.
     * 
     * @throws std::runtime_error if the shape is not recognized.
     */
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

    /**
     * @brief Checks if a JSON field is missing or null and records an error.
     * 
     * @param j JSON object or subfield.
     * @param path Description of the field (used in error message).
     * @param error ConfigError object to accumulate error messages.
     */
    void check_json_field(const json& j, const std::string& path, ConfigError& error) {
        if (j.is_null()) {
            error.add_error("Error: Required configuration value '" + path + "' is null or missing");
        }
    }

    /// Validates required JSON configuration fields. (Implementation not shown here)
    void validate_config(const json& config, ConfigError& error);

private:
    /// Helper function to create a RegularSlab material.
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

    /// Helper function to create a Sphere material.
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

    /// Helper function to create a FiniteSlab material.
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

    /// Helper function to create a DoubleSlab material.
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

#endif // MATERIALFACTORY_HPP
