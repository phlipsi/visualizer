#include "parameters.h"

#include <stdexcept>

namespace visualizer {

void Parameters::set_time(unsigned long ms) {
    for (auto &entry : parameters) {
        entry.second.set_time(ms);
    }
}

void Parameters::add_action(const std::string &name, unsigned long timestamp, std::unique_ptr<Action> action) {
    parameters[name].add_action(timestamp, std::move(action));
}

const float &Parameters::get_parameter(const std::string &name) {
    const auto it = parameters.find(name);
    if (it == parameters.end()) {
        throw std::runtime_error("Undefined parameter");
    }
    return it->second.get_value();
}

}
