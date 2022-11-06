#pragma once

#include "parameter.h"

#include <istream>
#include <map>
#include <memory>
#include <string>

namespace visualizer {

class Action;

class Parameters {
public:
    explicit Parameters(std::istream &input);

    void set_time(float t);

    void add_action(const std::string &name, float t, std::unique_ptr<Action> action);

    const float &get_parameter(const std::string &name);

    float get_ms_per_beat() const { return ms_per_beat; }
private:
    float ms_per_beat;
    std::map<std::string, Parameter> parameters;
};

}
