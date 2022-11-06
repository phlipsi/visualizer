#pragma once

#include "action.h"

#include <map>
#include <memory>

namespace visualizer {

class Parameter {
public:
    void set_time(unsigned long ms);
    void add_action(unsigned long timestamp, std::unique_ptr<Action> action);

    const float &get_value() const { return value; }
private:
    typedef std::map<unsigned long, std::unique_ptr<Action>> ActionMap;

    float value;
    ActionMap actions;

    ActionMap::const_iterator get_current(unsigned long ms) const;
};

}
