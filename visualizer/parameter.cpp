#include "parameter.h"

#include <stdexcept>

namespace visualizer {

void Parameters::set_time(long ms) {
    for (const auto &entry : modifiers) {
        parameters[entry.first] = entry.second(ms);
    }
}

void Parameters::add_parameter(const std::string &name, std::function<float(long)> modifier) {
    modifiers[name] = modifier;
    parameters[name] = 0.0f;
}

const float &Parameters::get_parameter(const std::string &name) {
    const auto it = parameters.find(name);
    if (it == parameters.end()) {
        throw std::runtime_error("Undefined parameter");
    }
    return it->second;
}

}
