#include "parameters.h"

#include <SDL_stdinc.h>
#include <nlohmann/json.hpp>

#include <cmath>
#include <stdexcept>

namespace visualizer {

Parameters::Parameters(std::istream &input) {
    const auto choreography = nlohmann::json::parse(input);
    const auto general = choreography["general"];
    const float bpm = general["bpm"].get<float>();
    const std::string meter = general["meter"].get<std::string>();
    const std::string::size_type slash = meter.find('/');
    int meter_num = 0;
    int meter_denum = 0;
    if (slash != std::string::npos) {
        meter_num = std::stoi(meter.substr(0, slash));
        meter_denum = std::stoi(meter.substr(slash + 1));
    }
    ms_per_measure = static_cast<float>(meter_num) * 60000.0f / bpm;

    const auto parameters = choreography["parameters"];
    for (const auto &entry : parameters.items()) {
        const std::string name = entry.key();
        for (const auto &timestamp : entry.value().items()) {
            const float measure = std::stof(timestamp.key());
            const auto &values = timestamp.value();
            if (values.contains("transition")) {
                const auto transition = values["transition"];
                this->parameters[name].add_transition(measure + transition["start"].get<float>(),
                                                      measure + transition["end"].get<float>());
            }
            this->parameters[name].add_action(measure, create_action(measure, values["action"]));
        }
    }
}

void Parameters::set_measure(float measure) {
    for (auto &entry : parameters) {
        entry.second.set_measure(measure);
    }
}

void Parameters::add_action(const std::string &name, float measure, std::unique_ptr<Action> action) {
    parameters[name].add_action(measure, std::move(action));
}

const float &Parameters::get_parameter(const std::string &name) {
    const auto it = parameters.find(name);
    if (it == parameters.end()) {
        throw std::runtime_error("Undefined parameter " + name);
    }
    return it->second.get_value();
}

}
