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

float psi(float x) {
    if (x <= 0.0f) {
        return 0.0f;
    } else {
        //return x * x * x;
        return x * x;
    }
}

float phi(float x) {
    if (x <= 0.0f) {
        return 0.0f;
    } else if (x > 0.0f && x < 1.0f) {
        return psi(x) / (psi(x) + psi(1.0f - x));
    } else {
        return 1.0f;
    }
}

}

void Parameter::set_measure(float measure) {
    Actions::const_iterator current = find_last_less_than_or_equal(actions.begin(), actions.end(), measure);
    if (current == actions.end()) {
        throw std::runtime_error("Missing initial action");
    }

    const Transitions::const_iterator transition = find_last_less_than_or_equal(transitions.begin(), transitions.end(), measure);
    if (transition != transitions.end() && transition->get_end() > measure) {
        float start_value = 0.0f;
        float end_value = 0.0f;
        if ((*current)->get_start() >= transition->get_start()) {
            //            --.---.---.-|-.---.---
            // Action       p       c |
            // Transition       b     | e
            const auto previous = std::prev(current);
            start_value = (*previous)->get_value(measure);
            end_value = (*current)->get_value(measure);
        } else {
            //            --.---.-|-.---.---.---
            // Action       c     | n
            // Transition       b |     e
            const auto next = std::next(current);
            start_value = (*current)->get_value(measure);
            end_value = (*next)->get_value(measure);
        }

        //start_value = start_value - transition->get_modulus() * std::floor(start_value / transition->get_modulus());
        //end_value = end_value - transition->get_modulus() * std::floor(end_value / transition->get_modulus());
        const float t = (measure - transition->get_start()) / (transition->get_end() - transition->get_start());
        value = phi(1 - t) * start_value + phi(t) * end_value;
    } else {
        value = (*current)->get_value(measure);
    }
}

void Parameter::add_action(float timestamp, std::unique_ptr<Action> action) {
    actions.emplace(std::move(action));
}

void Parameter::add_transition(float start, float end) {
    transitions.emplace(Transition{ start, end });
}

}
