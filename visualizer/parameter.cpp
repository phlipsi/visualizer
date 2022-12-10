#include "parameter.h"

#include <nlohmann/json.hpp>

namespace visualizer {

Parameter::Parameter(const nlohmann::json &actions)
  : value(0.0f)
{
    for (const auto &item : actions.items()) {
        const float time = std::stof(item.key());
        this->actions.emplace_back(create_action(time, item.value()));
    }
}

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
    const Actions::const_iterator current = find_last_less_than_or_equal(actions.begin(), actions.end(), measure);
    if (current == actions.end()) {
        value = actions.front()->get_value(measure);
    } else {
        value = (*current)->get_value(measure);
    }
}

}
