#ifndef CONFIGERROR_HPP
#define CONFIGERROR_HPP

#include <vector>
#include <string>
#include <iostream>

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

#endif 
