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

    void set_measure(float measure);

    void add_action(const std::string &name, float measure, std::unique_ptr<Action> action);

    const float &get_parameter(const std::string &name);

    float get_ms_per_measure() const { return ms_per_measure; }
private:
    float ms_per_measure;
    std::map<std::string, Parameter> parameters;
};

}
