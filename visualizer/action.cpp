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

Sine::Sine(float measures, float start, float baseline, float amplitude)
  : measures(measures),
    start(start),
    baseline(baseline),
    amplitude(amplitude)
{ }

float Sine::get_value(float measure,
                      float previous_end,
                      const Action *previous,
                      float next_start,
                      const Action *next) const
{
    return amplitude * sinf(2 * static_cast<float>(M_PI) * (measure + start) / measures) + baseline;
}

Sawtooth::Sawtooth(float measures, float start, float baseline, float amplitude)
  : measures(measures),
    start(start),
    baseline(baseline),
    amplitude(amplitude)
{ }

namespace {

float sawtooth(float t) {
    float scale = 2.0f / static_cast<float>(M_PI);
    float sum = sinf(static_cast<float>(M_PI) * t);
    sum -= sinf(2 * static_cast<float>(M_PI) * t) / 2.0f;
    sum += sinf(3 * static_cast<float>(M_PI) * t) / 3.0f;
    sum -= sinf(4 * static_cast<float>(M_PI) * t) / 4.0f;
    return scale * sum;
}

}

float Sawtooth::get_value(float measure,
                          float previous_end,
                          const Action *previous,
                          float next_start,
                          const Action *next) const
{
    return amplitude * sawtooth((measure + start) / measures) + baseline;
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

float square(float t) {
    float result = 0;
    result += sinf(2 * static_cast<float>(M_PI) * 1.0f * t) / 1.0f;
    result += sinf(2 * static_cast<float>(M_PI) * 3.0f * t) / 3.0f;
    result += sinf(2 * static_cast<float>(M_PI) * 5.0f * t) / 5.0f;
    return 4.0f * result / static_cast<float>(M_PI);
}

float sawtoothd(float t) {
    return t - 2 * std::floor((t - 1.0f) / 2.0f) - 2.0f;
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

std::unique_ptr<Action> create_action(const std::string &name, const std::vector<float> &params) {
    if (name == "constant") {
        return std::make_unique<Constant>(params.at(0));
    } else if (name == "linear") {
        return std::make_unique<Linear>();
    } else if (name == "smooth") {
        return std::make_unique<Smooth>();
    } else if (name == "sine") {
        return std::make_unique<Sine>(params.at(0), params.at(1), params.at(2), params.at(3));
    } else if (name == "sawtooth") {
        return std::make_unique<Sawtooth>(params.at(0), params.at(1), params.at(2), params.at(3));
    } else if (name == "steady") {
        return std::make_unique<Steady>(params.at(0), params.at(1));
    } else if (name == "square") {
        return std::make_unique<Periodic>(square, params.at(0), params.at(1), params.at(2), params.at(3));
    } else if (name == "sawtoothd") {
        return std::make_unique<Periodic>(sawtoothd, params.at(0), params.at(1), params.at(2), params.at(3));
    } else if (name == "step") {
        return std::make_unique<Step>(params.at(0), std::vector<float>(params.begin() + 1, params.end()));
    } else {
        throw std::runtime_error("Unknown action " + name);
    }
}

}
