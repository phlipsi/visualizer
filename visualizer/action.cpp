#include "action.h"

#include <SDL_stdinc.h>

#include <cmath>
#include <stdexcept>

namespace visualizer {

Action::Action(float start) : start(start) {  }

float Action::get_derivative(float measure) const {
    return (get_value(measure + 0.1) - get_value(measure)) / 0.1f;
}

class Constant : public Action {
public:
    Constant(float start, float value)
      : Action(start),
        value(value) { }

    float get_value(float measure) const override {
        return value;
    }

    float get_derivative(float measure) const {
        return 0.0f;
    }

private:
    float value;
};

class Periodic : public Action {
public:
    Periodic(float start,
             std::function<float(float)> func,
             std::function<float(float)> dfunc,
             float period,
             float phase,
             float offset,
             float amplitude)
      : Action(start),
        func(func),
        dfunc(dfunc),
        period(period),
        phase(phase),
        offset(offset),
        amplitude(amplitude)
    { }

    float get_value(float measure) const override {
        return amplitude * func((measure - get_start() + phase) / period) + offset;
    }

    float get_derivative(float measure) const override {
        return amplitude * dfunc((measure - get_start() + phase) / period) / period;
    }
private:
    std::function<float(float)> func;
    std::function<float(float)> dfunc;
    float period;
    float phase;
    float offset;
    float amplitude;
};

class Linear : public Action {
public:
    Linear(float start, float increase_per_measure, float initial_value)
      : Action(start),
        increase_per_measure(increase_per_measure),
        initial_value(initial_value)
    { }

    float get_value(float measure) const override {
        return (measure - get_start()) * increase_per_measure + initial_value;
    }

    float get_derivative(float measure) const override {
        return increase_per_measure;
    }

private:
    float increase_per_measure;
    float initial_value;

};

class Step : public Action {
public:
    Step(float start, float length, const std::vector<float> &values)
      : Action(start),
        length(length),
        values(values)
    { }

    float get_value(float measure) const override {
        const auto pos = static_cast<int>((measure - get_start()) / length) % values.size();
        return values[pos];
    }

    float get_derivative(float measure) const override {
        return 0.0f;
    }

private:
    float length;
    std::vector<float> values;
};

namespace {

float fourier_square(float t) {
    float result = 0;
    result += sinf(2 * static_cast<float>(M_PI) * 1.0f * t) / 1.0f;
    result += sinf(2 * static_cast<float>(M_PI) * 3.0f * t) / 3.0f;
    result += sinf(2 * static_cast<float>(M_PI) * 5.0f * t) / 5.0f;
    return 4.0f * result / static_cast<float>(M_PI);
}

float d_fourier_square(float t) {
    float result = 0;
    result += 2 * static_cast<float>(M_PI) * 1.0f * cosf(2 * static_cast<float>(M_PI) * 1.0f * t) / 1.0f;
    result += 2 * static_cast<float>(M_PI) * 3.0f * cosf(2 * static_cast<float>(M_PI) * 3.0f * t) / 3.0f;
    result += 2 * static_cast<float>(M_PI) * 5.0f * cosf(2 * static_cast<float>(M_PI) * 5.0f * t) / 5.0f;
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

float d_fourier_sawtooth(float t) {
    float scale = 2.0f / static_cast<float>(M_PI);
    float sum = static_cast<float>(M_PI) * cosf(static_cast<float>(M_PI) * t);
    sum -= 2 * static_cast<float>(M_PI) * cosf(2 * static_cast<float>(M_PI) * t) / 2.0f;
    sum += 3 * static_cast<float>(M_PI) * cosf(3 * static_cast<float>(M_PI) * t) / 3.0f;
    sum -= 4 * static_cast<float>(M_PI) * cosf(4 * static_cast<float>(M_PI) * t) / 4.0f;
    return scale * sum;
}

float sawtooth(float t) {
    return 2.0f * t - 2.0f * std::floor((2.0f * t - 1.0f) / 2.0f) - 2.0f;
}

float d_sawtooth(float t) {
    return 2.0f;
}

float sine(float t) {
    return std::sin(2 * static_cast<float>(M_PI) * t);
}

float d_sine(float t) {
    return 2 * static_cast<float>(M_PI) * std::cos(2 * static_cast<float>(M_PI) * t);
}

float norm_exp(float alpha, float t) {
    return (1.0f - exp(-alpha * t)) / (1.0f - exp(-alpha));
}

float d_norm_exp(float alpha, float t) {
    return alpha * exp(-alpha * t) / (1.0f - exp(-alpha));
}

float exp_sawtooth_wave(float t) {
    const float s = sawtooth(t);
    const float alpha = 2.0f;
    if (s >= 0.0f) {
        return norm_exp(alpha, s);
    } else {
        return 1.0f - norm_exp(alpha, s + 1);
    }
}

float d_exp_sawtooth_wave(float t) {
    const float s = sawtooth(t);
    const float alpha = 2.0f;
    if (s >= 0.0f) {
        return d_norm_exp(alpha, s);
    } else {
        return - d_norm_exp(alpha, s + 1);
    }
}

}

class ExpSawtooth : public Action {
public:
    ExpSawtooth(float start, float period, float phase, float slope, float height, float offset)
        : Action(start),
        period(period),
        phase(phase),
        slope(slope),
        height(height),
        offset(offset) { }

    float get_value(float measure) const override {
        const float s = measure - get_start() + phase;
        const float t = s - period * std::floor(s / period);
        //const float t = 0.5f * sawtooth((measure - get_start() + phase) / period) + 1.0f;
        return height * std::exp(-slope * t) + offset;
    }

    float get_derivative(float measure) const override {
        const float s = measure - get_start() + phase;
        const float t = s - period * std::floor(s / period);
        return -slope * height * std::exp(-slope * t);
    }

private:
    float period;
    float phase;
    float slope;
    float height;
    float offset;
};

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
        return std::make_unique<Periodic>(start, sine, d_sine, period, phase, offset, amplitude);
    } else if (name == "constant") {
        const float value = parameters["value"].get<float>();
        return std::make_unique<Constant>(start, value);
    } else if (name == "fourier-square") {
        const float period = parameters["period"].get<float>();
        const float phase = parameters.value<float>("phase", 0.0f);
        const float offset = parameters.value<float>("offset", 0.0f);
        const float amplitude = parameters.value<float>("amplitude", 1.0f);
        return std::make_unique<Periodic>(start, fourier_square, d_fourier_square, period, phase, offset, amplitude);
    } else if (name == "sawtooth") {
        const float period = parameters["period"].get<float>();
        const float phase = parameters.value<float>("phase", 0.0f);
        const float offset = parameters.value<float>("offset", 0.0f);
        const float amplitude = parameters.value<float>("amplitude", 1.0f);
        return std::make_unique<Periodic>(start, sawtooth, d_sawtooth, period, phase, offset, amplitude);
    } else if (name == "exp-sawtooth") {
        const float period = parameters["period"].get<float>();
        const float phase = parameters.value<float>("phase", 0.0f);
        const float slope = parameters.value<float>("slope", 1.0f);
        const float height = parameters.value<float>("height", 1.0f);
        const float offset = parameters.value<float>("offset", 0.0f);
        return std::make_unique<ExpSawtooth>(start, period, phase, slope, height, offset);
    } else if (name == "fourier-sawtooth") {
        const float period = parameters["period"].get<float>();
        const float phase = parameters.value<float>("phase", 0.0f);
        const float offset = parameters.value<float>("offset", 0.0f);
        const float amplitude = parameters.value<float>("amplitude", 1.0f);
        return std::make_unique<Periodic>(start, fourier_sawtooth, d_fourier_sawtooth, period, phase, offset, amplitude);
    } else if (name == "exp-sawtooth-wave") {
        const float period = parameters["period"].get<float>();
        const float phase = parameters.value<float>("phase", 0.0f);
        const float offset = parameters.value<float>("offset", 0.0f);
        const float amplitude = parameters.value<float>("amplitude", 1.0f);
        return std::make_unique<Periodic>(start, exp_sawtooth_wave, d_exp_sawtooth_wave, period, phase, offset, amplitude);
    } else if (name == "linear") {
        const float increase = parameters["increase"].get<float>();
        const float initial = parameters.value<float>("initial", 0.0f);
        return std::make_unique<Linear>(start, increase, initial);
    } else {
        throw std::runtime_error("Unknown action " + name);
    }
}

}
