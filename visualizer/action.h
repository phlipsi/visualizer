#pragma once

#include <nlohmann/json.hpp>

#include <functional>
#include <memory>
#include <vector>

namespace visualizer {

class Action {
public:
    explicit Action(float start);

    float get_start() const { return start; }

    virtual float get_value(float measure) const = 0;
    virtual float get_derivative(float measure) const;

    Action(const Action &) = delete;
    Action(Action &&) = delete;
    Action &operator = (const Action &) = delete;
    Action &operator = (Action &&) = delete;

private:
    float start;
};

inline bool operator < (float measure, const std::unique_ptr<Action> &pa) {
    if (!pa) {
        throw std::runtime_error("Can't compare to a null pointer");
    }
    return measure < pa->get_start();
}

inline bool operator < (const std::unique_ptr<Action> &pa, float measure) {
    if (!pa) {
        throw std::runtime_error("Can't compare to a null pointer");
    }
    return pa->get_start() < measure;
}

inline bool operator < (const std::unique_ptr<Action> &pa, const std::unique_ptr<Action> &pb) {
    if (!pa || !pb) {
        throw std::runtime_error("Can't compare to a null pointer");
    }
    return pa->get_start() < pb->get_start();
}

std::unique_ptr<Action> create_action(float start, const nlohmann::json &action);

}
