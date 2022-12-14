#pragma once

#include "action.h"

#include <nlohmann/json_fwd.hpp>

#include <memory>
#include <vector>

namespace visualizer {

class Parameter {
public:
    explicit Parameter(const nlohmann::json &actions);
    void load(const nlohmann::json &actions);
    void clear();

    void set_measure(float measure);
    const float &get_value() const { return value; }

private:
    using Actions = std::vector<std::unique_ptr<Action>>;
    Actions actions;
    float value;
};

}
