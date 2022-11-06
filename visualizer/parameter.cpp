#include "parameter.h"

#include <stdexcept>

namespace visualizer {

Parameter::ActionMap::const_iterator Parameter::get_current(float t) const {
    auto result = actions.end();
    for (auto it = actions.begin(); it != actions.end(); ++it) {
        if (it->first > t) {
            break;
        } else {
            result = it;
        }
    }
    return result;
}


void Parameter::set_time(float t) {
    const auto current = get_current(t);
    if (current == actions.end()) {
        throw std::runtime_error("Missing initial action");
    }
    const Action *previous = nullptr;
    unsigned long previous_end = 0;
    if (current != actions.begin()) {
        const auto previous_it = std::prev(current);
        previous = previous_it->second.get();
        previous_end = current->first - previous_it->first;
    }
    const Action *next = nullptr;
    const auto next_it = std::next(current);
    unsigned long next_start = 0;
    if (next_it != actions.end()) {
        next = next_it->second.get();
        next_start = next_it->first - current->first;
    }
    value = current->second->get_value(t - current->first, previous_end, previous, next_start, next);
}

void Parameter::add_action(float timestamp, std::unique_ptr<Action> action) {
    actions.emplace(timestamp, std::move(action));
}

}
