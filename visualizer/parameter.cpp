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

float newton_interpolation(float a, float fa, float faa, float b, float fb, float fbb, float x) {
    const float fab = (fb - fa) / (b - a);
    const float faab = (fab - faa) / (b - a);
    const float fabb = (fbb - fab) / (b - a);
    const float faabb = (fabb - faab) / (b - a);
    return fa + faa * (x - a) + faab * (x - a) * (x - a) + faabb * (x - a) * (x - a) * (x - b);
}

}

void Parameter::set_measure(float measure) {
    Actions::const_iterator current = find_last_less_than_or_equal(actions.begin(), actions.end(), measure);
    if (current == actions.end()) {
        throw std::runtime_error("Missing initial action");
    }

    const Transitions::const_iterator transition = find_last_less_than_or_equal(transitions.begin(), transitions.end(), measure);
    if (transition != transitions.end() && transition->get_end() > measure) {
        const float start = transition->get_start();
        const float end = transition->get_end();
        float start_value = 0.0f;
        float start_derivative = 0.0f;
        float end_value = 0.0f;
        float end_derivative = 0.0f;
        if ((*current)->get_start() >= transition->get_start()) {
            //            --.---.---.-|-.---.---
            // Action       p       c |
            // Transition       b     | e
            const auto previous = std::prev(current);
            start_value = (*previous)->get_value(start);
            start_derivative = (*previous)->get_derivative(start);
            end_value = (*current)->get_value(end);
            end_derivative = (*current)->get_derivative(end);
        } else {
            //            --.---.-|-.---.---.---
            // Action       c     | n
            // Transition       b |     e
            const auto next = std::next(current);
            start_value = (*current)->get_value(start);
            start_derivative = (*current)->get_derivative(start);
            end_value = (*next)->get_value(end);
            end_derivative = (*next)->get_derivative(end);
        }

        //start_value = start_value - transition->get_modulus() * std::floor(start_value / transition->get_modulus());
        //end_value = end_value - transition->get_modulus() * std::floor(end_value / transition->get_modulus());
        value = newton_interpolation(start, start_value, start_derivative,
                                     end, end_value, end_derivative, measure);
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
