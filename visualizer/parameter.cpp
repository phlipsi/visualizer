#include "parameter.h"

#include <stdexcept>

namespace visualizer {

namespace {

template<typename V>
typename std::map<float, V>::const_iterator get_current(const std::map<float, V> &m, float t) {
    auto result = m.end();
    for (auto it = m.begin(); it != m.end(); ++it) {
        if (it->first > t) {
            break;
        } else {
            result = it;
        }
    }
    return result;
}

float psi(float x) {
    if (x <= 0.0f) {
        return 0.0f;
    } else {
        return x * x * x;
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
    ActionMap::const_iterator current = get_current(actions, measure);
    if (current == actions.end()) {
        throw std::runtime_error("Missing initial action");
    }

    const TransitionMap::const_iterator transition = get_current(transitions, measure);
    if (transition != transitions.end() && transition->second > measure) {
        if (current->first >= transition->first) {
            //            --.---.---.-|-.---.---
            // Action       p       c |
            // Transition       b     | e
            const auto previous = std::prev(current);
            const float current_value = current->second->get_value(measure - current->first, 0, nullptr, 0, nullptr);
            const float previous_value = previous->second->get_value(measure - previous->first, 0, nullptr, 0, nullptr);
            const float t = (measure - transition->first) / (transition->second - transition->first);
            value = phi(1 - t) * previous_value + phi(t) * current_value;
        } else {
            //            --.---.-|-.---.---.---
            // Action       c     | n
            // Transition       b |     e
            const auto next = std::next(current);
            const float current_value = current->second->get_value(measure - current->first, 0, nullptr, 0, nullptr);
            const float next_value = next->second->get_value(measure - next->first, 0, nullptr, 0, nullptr);
            const float t = (measure - transition->first) / (transition->second - transition->first);
            value = phi(1 - t) * current_value + phi(t) * next_value;
        }
    } else {
        value = current->second->get_value(measure - current->first, 0, nullptr, 0, nullptr);
    }
}

void Parameter::add_action(float timestamp, std::unique_ptr<Action> action) {
    actions.emplace(timestamp, std::move(action));
}

void Parameter::add_transition(float start, float end) {
    transitions.emplace(start, end);
}

}
