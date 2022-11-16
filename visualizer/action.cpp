#include "action.h"

#include <SDL_stdinc.h>

#include <cmath>
#include <stdexcept>

namespace visualizer {

Action::Action(float start) : start(start) {  }

Constant::Constant(float start, float value) : Action(start), value(value) { }

float Constant::get_value(float measure) const {
    return value;
}

Periodic::Periodic(float start, std::function<float(float)> func, float period, float phase, float offset, float amplitude)
  : Action(start),
    func(func),
    period(period),
    phase(phase),
    offset(offset),
    amplitude(amplitude)
{ }

float Periodic::get_value(float measure) const {
    return amplitude * func((measure - get_start() + phase) / period) + offset;
}

Linear::Linear(float start, float increase_per_measure, float initial_value)
  : Action(start),
    increase_per_measure(increase_per_measure),
    initial_value(initial_value)
{ }

float Linear::get_value(float measure) const {
    return (measure - get_start()) * increase_per_measure + initial_value;
}

namespace {

float fourier_square(float t) {
    float result = 0;
    result += sinf(2 * static_cast<float>(M_PI) * 1.0f * t) / 1.0f;
    result += sinf(2 * static_cast<float>(M_PI) * 3.0f * t) / 3.0f;
    result += sinf(2 * static_cast<float>(M_PI) * 5.0f * t) / 5.0f;
    return 4.0f * result / static_cast<float>(M_PI);
}

float fourier_sawtooth(float t) {
    float scale = 2.0f / static_cast<float>(M_PI);
    float sum = sinf(static_cast<float>(M_PI) * t);
    sum -= sinf(2 * static_cast<float>(M_PI) * t) / 2.0f;
    sum += sinf(3 * static_cast<float>(M_PI) * t) / 3.0f;
    sum -= sinf(4 * static_cast<float>(M_PI) * t) / 4.0f;
    return scale * sum;
}

float sawtooth(float t) {
    return t - 2 * std::floor((t - 1.0f) / 2.0f) - 2.0f;
}

float sine(float t) {
    return std::sin(2 * static_cast<float>(M_PI) * t);
}

}

Step::Step(float start, float length, const std::vector<float> &values)
  : Action(start), length(length), values(values) { }

float Step::get_value(float measure) const
{
    const auto pos = static_cast<int>((measure - get_start()) / length) % values.size();
    return values[pos];
}

std::unique_ptr<Action> create_action(float start, const nlohmann::json &action) {
    const std::string name = action["name"].get<std::string>();
    const auto &parameters = action["parameters"];
    if (name == "step") {
        const float length = parameters["length"].get<float>();
        const std::vector<float> params = parameters["values"].get<std::vector<float>>();
        return std::make_unique<Step>(start, length, params);
    } else if (name == "sine") {
        const float period = parameters["period"].get<float>();
        const float phase = parameters.value<float>("phase", 0.0f);
        const float offset = parameters.value<float>("offset", 0.0f);
        const float amplitude = parameters.value<float>("amplitude", 1.0f);
        return std::make_unique<Periodic>(start, sine, period, phase, offset, amplitude);
    } else if (name == "constant") {
        const float value = parameters["value"].get<float>();
        return std::make_unique<Constant>(start, value);
    } else if (name == "fourier-square") {
        const float period = parameters["period"].get<float>();
        const float phase = parameters.value<float>("phase", 0.0f);
        const float offset = parameters.value<float>("offset", 0.0f);
        const float amplitude = parameters.value<float>("amplitude", 1.0f);
        return std::make_unique<Periodic>(start, fourier_square, period, phase, offset, amplitude);
    } else if (name == "sawtooth") {
        const float period = parameters["period"].get<float>();
        const float phase = parameters.value<float>("phase", 0.0f);
        const float offset = parameters.value<float>("offset", 0.0f);
        const float amplitude = parameters.value<float>("amplitude", 1.0f);
        return std::make_unique<Periodic>(start, sawtooth, period, phase, offset, amplitude);
    } else if (name == "linear") {
        const float increase = parameters["increase"].get<float>();
        const float start = parameters.value<float>("start", 0.0f);
        return std::make_unique<Linear>(start, increase, start);
    } else {
        throw std::runtime_error("Unknown action " + name);
    }
}

}
