#pragma once

#include "action.h"

#include <map>
#include <memory>

namespace visualizer {

class Parameter {
public:
    void set_measure(float measure);
    void add_action(float measure, std::unique_ptr<Action> action);
    void add_transition(float start, float end);

    const float &get_value() const { return value; }
private:
    typedef std::map<float, std::unique_ptr<Action>> ActionMap;
    typedef std::map<float, float> TransitionMap;

    float value;
    ActionMap actions;
    TransitionMap transitions;
};

}
