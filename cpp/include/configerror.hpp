#ifndef CONFIGERROR_HPP
#define CONFIGERROR_HPP

#include <vector>
#include <string>
#include <iostream>

/**
 * @brief A simple structure to collect and report configuration errors.
 * 
 * This struct is used to store error messages during the configuration or
 * initialization phase of a program (e.g., when loading input parameters or settings).
 */
struct ConfigError {
    std::vector<std::string> errors; ///< Container for storing error messages

    /**
     * @brief Add a new error message to the list.
     * 
     * @param error A descriptive string of the error
     */
    void add_error(const std::string& error) {
        errors.push_back(error);
    }

    /**
     * @brief Check if any errors have been recorded.
     * 
     * @return true if there are errors, false otherwise
     */
    bool has_errors() const {
        return !errors.empty();
    }

    /**
     * @brief Print all recorded errors to the standard error output.
     * 
     * This is useful for quickly debugging configuration issues.
     */
    void print_errors() const {
        for (const auto& err : errors) {
            std::cerr << err << std::endl;
        }
    }
};

#endif // CONFIGERROR_HPP
