#include "action.h"

#include <SDL_stdinc.h>

#include <cmath>
#include <stdexcept>

namespace visualizer {

Constant::Constant(float value) : value(value) { }

float Constant::get_value(float measure,
                          float previous_end,
                          const Action *previous,
                          float next_start,
                          const Action *next) const
{
    return value;
}

float Linear::get_value(float measure,
                        float previous_end,
                        const Action *previous,
                        float next_start,
                        const Action *next) const
{
    if (previous == nullptr) {
        throw std::runtime_error("Linear interpolation needs a previous action");
    }
    if (next == nullptr) {
        throw std::runtime_error("Linear interpolation needs a next action");
    }
    const float start = previous->get_value(previous_end, 0, nullptr, 0, nullptr);
    const float end = next->get_value(0, 0, nullptr, 0, nullptr);
    const float t = measure / next_start;
    return (1.0f - t) * start + t * end;
}

namespace {

float smooth(float t) {
    if (t <= 0.0f) {
        return 0.0f;
    } else if (t >= 1.0f) {
        return 1.0f;
    } else {
        return t * t * (3.0f - 2.0f * t);
    }
}

}

float Smooth::get_value(float measure,
                        float previous_end,
                        const Action *previous,
                        float next_start,
                        const Action *next) const
{
    if (previous == nullptr) {
        throw std::runtime_error("Linear interpolation needs a previous action");
    }
    if (next == nullptr) {
        throw std::runtime_error("Linear interpolation needs a next action");
    }
    const float start = previous->get_value(previous_end, 0, nullptr, 0, nullptr);
    const float end = next->get_value(0, 0, nullptr, 0, nullptr);
    const float t = smooth(measure / next_start);
    return (1.0f - t) * start + t * end;
}

Periodic::Periodic(std::function<float(float)> func, float measures, float start, float baseline, float amplitude)
  : func(func),
    measures(measures),
    start(start),
    baseline(baseline),
    amplitude(amplitude)
{ }

float Periodic::get_value(float measure,
                          float previous_end,
                          const Action *previous,
                          float next_start,
                          const Action *next) const
{
    return amplitude * func((measure + start) / measures) + baseline;
}

Steady::Steady(float increase_per_measure, float start)
  : increase_per_measure(increase_per_measure),
    start(start)
{ }

float Steady::get_value(float measure,
                        float previous_end,
                        const Action *previous,
                        float next_start,
                        const Action *next) const
{
    return measure * increase_per_measure + start;
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

Step::Step(float length, const std::vector<float> &values)
    : length(length), values(values) { }

float Step::get_value(float measure,
                      float previous_end,
                      const Action *previous,
                      float next_start,
                      const Action *next) const
{
    const auto pos = static_cast<int>(measure / length) % values.size();
    return values[pos];
}

std::unique_ptr<Action> create_action(const nlohmann::json &action) {
    const std::string name = action["name"].get<std::string>();
    const auto &parameters = action["parameters"];
    if (name == "step") {
        const float length = parameters["length"].get<float>();
        const std::vector<float> params = parameters["values"].get<std::vector<float>>();
        return std::make_unique<Step>(length, params);
    } else if (name == "sine") {
        const float period = parameters["period"].get<float>();
        const float phase = parameters.value<float>("phase", 0.0f);
        const float offset = parameters.value<float>("offset", 0.0f);
        const float amplitude = parameters.value<float>("amplitude", 1.0f);
        return std::make_unique<Periodic>(sine, period, phase, offset, amplitude);
    } else if (name == "constant") {
        const float value = parameters["value"].get<float>();
        return std::make_unique<Constant>(value);
    } else if (name == "fourier-square") {
        const float period = parameters["period"].get<float>();
        const float phase = parameters.value<float>("phase", 0.0f);
        const float offset = parameters.value<float>("offset", 0.0f);
        const float amplitude = parameters.value<float>("amplitude", 1.0f);
        return std::make_unique<Periodic>(fourier_square, period, phase, offset, amplitude);
    } else if (name == "sawtooth") {
        const float period = parameters["period"].get<float>();
        const float phase = parameters.value<float>("phase", 0.0f);
        const float offset = parameters.value<float>("offset", 0.0f);
        const float amplitude = parameters.value<float>("amplitude", 1.0f);
        return std::make_unique<Periodic>(sawtooth, period, phase, offset, amplitude);
    } else {
        throw std::runtime_error("Unknown action " + name);
    }
}

}
