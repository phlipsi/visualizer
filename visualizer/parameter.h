#pragma once

#include <functional>
#include <map>
#include <string>

namespace visualizer {

class Parameters {
public:
    void set_time(long ms);

    void add_parameter(const std::string &name, std::function<float(long)> modifier);

    const float &get_parameter(const std::string &name);

private:
    std::map<std::string, std::function<float(long)>> modifiers;
    std::map<std::string, float> parameters;
};

}
