#pragma once

#include "action.h"

#include <map>
#include <memory>

namespace visualizer {

class Parameter {
public:
    void set_measure(float measure);
    void add_action(float measure, std::unique_ptr<Action> action);

    const float &get_value() const { return value; }
private:
    typedef std::map<float, std::unique_ptr<Action>> ActionMap;

    float value;
    ActionMap actions;

    ActionMap::const_iterator get_current(float measure) const;
};

}
