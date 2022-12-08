#include "transition.h"

#include "action.h"

#include <cmath>
#include <stdexcept>

namespace visualizer {

Transition::Transition(float start, float end)
  : start(start),
    end(end)
{ }

namespace {

float newton_interpolation(float a, float fa, float faa, float b, float fb, float fbb, float x) {
    const float fab = (fb - fa) / (b - a);
    const float faab = (fab - faa) / (b - a);
    const float fabb = (fbb - fab) / (b - a);
    const float faabb = (fabb - faab) / (b - a);
    return fa + faa * (x - a) + faab * (x - a) * (x - a) + faabb * (x - a) * (x - a) * (x - b);
}

class Newton : public Transition {
public:
    Newton(float start, float end) : Transition(start, end) { }

    float get_value(float measure,
                    const Action &previous_action,
                    const Action &next_action) const override
    {
        const float start = get_start();
        const float end = get_end();
        const float start_value = previous_action.get_value(start);
        const float start_derivative = previous_action.get_derivative(start);
        const float end_value = next_action.get_value(end);
        const float end_derivative = next_action.get_derivative(end);
        return newton_interpolation(start, start_value, start_derivative,
                                    end, end_value, end_derivative,
                                    measure);
    }
};

class LeftSidedNewton : public Transition {
public:
    LeftSidedNewton(float start, float end, float end_derivative)
      : Transition(start, end),
        end_derivative(end_derivative)
    { }

    float get_value(float measure,
                    const Action &previous_action,
                    const Action &next_action) const override
    {
        const float start = get_start();
        const float end = get_end();
        const float start_value = previous_action.get_value(start);
        const float start_derivative = previous_action.get_derivative(end);
        const float end_value = next_action.get_value(get_end());
        return newton_interpolation(start, start_value, start_derivative,
                                    end, end_value, end_derivative,
                                    measure);
    }

private:
    float end_derivative;
};

float lerp(float t, float a, float b) {
    return (1 - t) * a + t * b;
}

class Linear : public Transition {
public:
    Linear(float start, float end) : Transition(start, end) { }

    float get_value(float measure,
                    const Action &previous_action,
                    const Action &next_action) const override
    {
        const float start = get_start();
        const float end = get_end();
        const float start_value = previous_action.get_value(start);
        const float end_value = next_action.get_value(end);
        return lerp((measure - start) / (end - start), start_value, end_value);
    }
};

}

std::unique_ptr<Transition> create_transition(float measure, const nlohmann::json &transition) {
    const float start = measure + transition["start"].get<float>();
    const float end = measure + transition["end"].get<float>();
    const std::string name = transition["name"].get<std::string>();
    const auto parameters = transition.find("parameters");
    if (name == "newton") {
        return std::make_unique<Newton>(start, end);
    } else if (name == "left-sided-newton") {
        const float end_derivative = parameters != transition.end() ? parameters->value<float>("end_derivative", 0.0f) : 0.0f;
        return std::make_unique<LeftSidedNewton>(start, end, end_derivative);
    } else if (name == "linear") {
        return std::make_unique<Linear>(start, end);
    } else {
        throw std::runtime_error("Unknown transition " + name);
    }
}

}
