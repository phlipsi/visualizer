#pragma once

#include "parameter.h"

#include <fstream>
#include <istream>
#include <map>
#include <memory>
#include <string>

namespace visualizer {

class Action;

class Parameters {
public:
    using ParameterMap = std::map<std::string, Parameter>;

    explicit Parameters(const std::string &filename);
    void load(const std::string &filename);
    void clear();

    void set_measure(float measure);
    void set_debug_output(const std::string &filename);

    void add_action(const std::string &name, std::unique_ptr<Action> action);

    const float &get_parameter(const std::string &name);

    float get_ms_per_measure() const { return ms_per_measure; }

    void choose_debugged_parameter();
    void plot_debugger_parameter(float measure, float around, int count);
private:
    std::ofstream debug_output;
    float ms_per_measure;
    ParameterMap parameters;
    ParameterMap::iterator debugged;
};

}
