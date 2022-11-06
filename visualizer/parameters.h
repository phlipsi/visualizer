#pragma once

#include "parameter.h"

#include <map>
#include <memory>
#include <string>

namespace visualizer {

class Action;

class Parameters {
public:
    void set_time(unsigned long ms);

    void add_action(const std::string &name, unsigned long timestamp, std::unique_ptr<Action> action);

    const float &get_parameter(const std::string &name);

private:
    std::map<std::string, Parameter> parameters;
};

}
