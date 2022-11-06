#include "action.h"

#include <cmath>
#include <stdexcept>

namespace visualizer {

Constant::Constant(float value) : value(value) { }

float Constant::get_value(unsigned long ms,
                          unsigned long previous_end,
                          const Action *previous,
                          unsigned long next_start,
                          const Action *next) const
{
    return value;
}

float Linear::get_value(unsigned long ms,
                        unsigned long previous_end,
                        const Action *previous,
                        unsigned long next_start,
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
    const float t = static_cast<float>(ms) / next_start;
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

float Smooth::get_value(unsigned long ms,
                        unsigned long previous_end,
                        const Action *previous,
                        unsigned long next_start,
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
    const float t = static_cast<float>(ms) / next_start;
    const float s = smooth(t);
    return (1.0f - s) * start + s * end;
}

Sine::Sine(float freq, float phase, float baseline, float amplitude)
  : freq(freq),
    phase(phase),
    baseline(baseline),
    amplitude(amplitude)
{ }

float Sine::get_value(unsigned long ms,
                      unsigned long previous_end,
                      const Action *previous,
                      unsigned long next_start,
                      const Action *next) const
{
    return amplitude * sinf(2 * static_cast<float>(M_PI) * (freq * static_cast<float>(ms) + phase)) + baseline;
}

Sawtooth::Sawtooth(float freq, float phase, float baseline, float amplitude)
  : freq(freq),
    phase(phase),
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

float Sawtooth::get_value(unsigned long ms,
                          unsigned long previous_end,
                          const Action *previous,
                          unsigned long next_start,
                          const Action *next) const
{
    return amplitude * sawtooth(freq * static_cast<float>(ms) + phase) + baseline;
}

}
