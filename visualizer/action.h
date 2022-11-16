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

class Constant : public Action {
public:
    Constant(float start, float value);

    float get_value(float measure) const override;

private:
    float value;
};

class Periodic : public Action {
public:
    Periodic(float start, std::function<float(float)> func, float period, float phase, float offset, float amplitude);

    float get_value(float measure) const override;

private:
    std::function<float(float)> func;
    float period;
    float phase;
    float offset;
    float amplitude;
};

class Linear : public Action {
public:
    Linear(float start, float increase_per_measure, float initial_value);

    float get_value(float measure) const override;

private:
    float increase_per_measure;
    float initial_value;

};

class Step : public Action {
public:
    Step(float start, float length, const std::vector<float> &values);

    float get_value(float measure) const override;

private:
    float length;
    std::vector<float> values;
};

std::unique_ptr<Action> create_action(float start, const nlohmann::json &action);

}
