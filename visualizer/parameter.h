#pragma once

#include "action.h"
#include "transition.h"

#include <set>
#include <memory>

namespace visualizer {

class Parameter {
public:
    void set_measure(float measure);
    void add_action(std::unique_ptr<Action> action);
    void add_transition(std::unique_ptr<Transition> transition);
    void clear();

    const float &get_value() const { return value; }

private:
    typedef std::set<std::unique_ptr<Action>> Actions;
    typedef std::set<std::unique_ptr<Transition>> Transitions;

    float value;
    Actions actions;
    Transitions transitions;
};

}
