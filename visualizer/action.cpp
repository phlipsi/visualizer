#include "action.h"

#include <cmath>
#include <stdexcept>

namespace visualizer {

Constant::Constant(float value) : value(value) { }

float Constant::get_value(float ms,
                          float previous_end,
                          const Action *previous,
                          float next_start,
                          const Action *next) const
{
    return value;
}

float Linear::get_value(float t,
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
    const float t1 = t / next_start;
    return (1.0f - t1) * start + t1 * end;
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

float Smooth::get_value(float t,
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
    const float t0 = t / next_start;
    const float s = smooth(t0);
    return (1.0f - s) * start + s * end;
}

Sine::Sine(float ms_per_beat, float beats, float start, float baseline, float amplitude)
  : freq(1.0f / (beats * ms_per_beat)),
    phase(ms_per_beat * start),
    baseline(baseline),
    amplitude(amplitude)
{ }

float Sine::get_value(float t,
                      float previous_end,
                      const Action *previous,
                      float next_start,
                      const Action *next) const
{
    return amplitude * sinf(2 * static_cast<float>(M_PI) * (freq * t + phase)) + baseline;
}

Sawtooth::Sawtooth(float ms_per_beat, float beats, float start, float baseline, float amplitude)
  : freq(1.0f / (beats * ms_per_beat)),
    phase(ms_per_beat * start),
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

float Sawtooth::get_value(float t,
                          float previous_end,
                          const Action *previous,
                          float next_start,
                          const Action *next) const
{
    return amplitude * sawtooth(freq * t + phase) + baseline;
}

Steady::Steady(float ms_per_beat, float increase_per_beat, float start_value)
  : slope(increase_per_beat / ms_per_beat),
    initial(start_value)
{ }

float Steady::get_value(float t,
                        float previous_end,
                        const Action *previous,
                        float next_start,
                        const Action *next) const
{
    return slope * t + initial;
}

std::unique_ptr<Action> create_action(const std::string &name, float ms_per_beat, const std::vector<float> &params) {
    if (name == "constant") {
        return std::make_unique<Constant>(params.at(0));
    } else if (name == "linear") {
        return std::make_unique<Linear>();
    } else if (name == "smooth") {
        return std::make_unique<Smooth>();
    } else if (name == "sine") {
        return std::make_unique<Sine>(ms_per_beat, params.at(0), params.at(1), params.at(2), params.at(3));
    } else if (name == "sawtooth") {
        return std::make_unique<Sawtooth>(ms_per_beat, params.at(0), params.at(1), params.at(2), params.at(3));
    } else if (name == "steady") {
        return std::make_unique<Steady>(ms_per_beat, params.at(0), params.at(1));
    } else {
        throw std::runtime_error("Unknown action " + name);
    }
}

}
