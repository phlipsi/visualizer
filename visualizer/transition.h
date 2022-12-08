#pragma once

#include <nlohmann/json.hpp>

#include <memory>

namespace visualizer {

class Action;

class Transition {
public:
    Transition(float start, float end);
    Transition(float start, float mid, float end);
    virtual ~Transition() = default;

    Transition(const Transition &) = delete;
    Transition(Transition &&) = delete;
    Transition &operator = (const Transition &) = delete;
    Transition &operator = (Transition &&) = delete;

    float get_start() const { return start; }
    float get_end() const { return end; }

    virtual float get_value(float measure,
                            const Action &previous_action,
                            const Action &next_action) const = 0;

private:
    float start;
    float end;
};

inline bool operator < (float measure, const std::unique_ptr<Transition> &pa) {
    if (!pa) {
        throw std::runtime_error("Can't compare to a null pointer");
    }
    return measure < pa->get_start();
}

inline bool operator < (const std::unique_ptr<Transition> &pa, float measure) {
    if (!pa) {
        throw std::runtime_error("Can't compare to a null pointer");
    }
    return pa->get_start() < measure;
}


inline bool operator < (const std::unique_ptr<Transition> &pa, const std::unique_ptr<Transition> &pb) {
    if (!pa || !pb) {
        throw std::runtime_error("Can't compare to a null pointer");
    }
    return pa->get_start() < pb->get_start();
}

std::unique_ptr<Transition> create_transition(float measure, const nlohmann::json &transition);

}
