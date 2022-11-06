#pragma once

#include <memory>
#include <vector>

namespace visualizer {

class Action {
public:
    Action() = default;
    virtual ~Action() = default;
    virtual float get_value(float t,
                            float previous_end,
                            const Action *previous,
                            float next_start,
                            const Action *next) const = 0;

    Action(const Action &) = delete;
    Action &operator = (const Action &) = delete;
};

class Constant : public Action {
public:
    Constant(float value);

    float get_value(float t,
                    float previous_end,
                    const Action *previous,
                    float next_start,
                    const Action *next) const override;

private:
    float value;
};

class Linear : public Action {
public:
    float get_value(float ms,
                    float previous_end,
                    const Action *previous,
                    float next_start,
                    const Action *next) const override;
};

class Smooth : public Action {
public:
    float get_value(float ms,
                    float previous_end,
                    const Action *previous,
                    float next_start,
                    const Action *next) const override;
};

class Sine : public Action {
public:
    Sine(float ms_per_beat, float beats, float start, float baseline, float amplitude);

    float get_value(float ms,
                    float previous_end,
                    const Action *previous,
                    float next_start,
                    const Action *next) const override;
private:
    float freq;
    float phase;
    float baseline;
    float amplitude;
};

class Sawtooth : public Action {
public:
    Sawtooth(float ms_per_beat, float beats, float start, float baseline, float amplitude);

    float get_value(float ms,
                    float previous_end,
                    const Action *previous,
                    float next_start,
                    const Action *next) const override;

private:
    float freq;
    float phase;
    float baseline;
    float amplitude;
};

class Steady : public Action {
public:
    Steady(float ms_per_beat, float increase_per_beat, float start_value);

    float get_value(float ms,
                    float previous_end,
                    const Action *previous,
                    float next_start,
                    const Action *next) const override;

private:
    float slope;
    float initial;

};

std::unique_ptr<Action> create_action(const std::string &name, float ms_per_beat, const std::vector<float> &params);

}
