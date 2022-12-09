#include "parameter.h"

#include <stdexcept>

namespace visualizer {

namespace {

template<typename Iter, typename Value>
Iter find_last_less_than_or_equal(Iter begin, Iter end, const Value &value) {
    if (begin == end) {
        return end;
    }
    const Iter result = std::upper_bound(begin, end, value);
    if (result == begin) {
        return end;
    } else {
        return std::prev(result);
    }
}

}

void Parameter::set_measure(float measure) {
    Actions::const_iterator current = find_last_less_than_or_equal(actions.begin(), actions.end(), measure);
    if (current == actions.end()) {
        throw std::runtime_error("Missing initial action");
    }

    const Transitions::const_iterator transition = find_last_less_than_or_equal(transitions.begin(), transitions.end(), measure);
    if (transition != transitions.end() && (*transition)->get_end() > measure) {
        const float start = (*transition)->get_start();
        const float end = (*transition)->get_end();
        Action *previous = nullptr;
        Action *next = nullptr;

        if ((*current)->get_start() >= (*transition)->get_start()) {
            //            --.---.---.-|-.---.---
            // Action       p       c |
            // Transition       b     | e
            previous = std::prev(current)->get();
            next = current->get();
        } else {
            //            --.---.-|-.---.---.---
            // Action       c     | n
            // Transition       b |     e
            previous = current->get();
            next = std::next(current)->get();
        }
        value = (*transition)->get_value(measure, *previous, *next);
    } else {
        value = (*current)->get_value(measure);
    }
}

void Parameter::add_action(std::unique_ptr<Action> action) {
    actions.emplace(std::move(action));
}

void Parameter::add_transition(std::unique_ptr<Transition> transition) {
    transitions.emplace(std::move(transition));
}

void Parameter::clear() {
    actions.clear();
    transitions.clear();
}

}
