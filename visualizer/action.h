#pragma once

#include <functional>
#include <memory>
#include <vector>

namespace visualizer {

class Action {
public:
    Action() = default;
    virtual ~Action() = default;

    virtual float get_value(float measure,
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

    float get_value(float measure,
                    float previous_end,
                    const Action *previous,
                    float next_start,
                    const Action *next) const override;

private:
    float value;
};

class Linear : public Action {
public:
    float get_value(float measure,
                    float previous_end,
                    const Action *previous,
                    float next_start,
                    const Action *next) const override;
};

class Smooth : public Action {
public:
    float get_value(float measure,
                    float previous_end,
                    const Action *previous,
                    float next_start,
                    const Action *next) const override;
};

class Periodic : public Action {
public:
    Periodic(std::function<float(float)> func, float measures, float start, float baseline, float amplitude);

    float get_value(float measure,
                    float previous_end,
                    const Action *previous,
                    float next_start,
                    const Action *next) const override;

private:
    std::function<float(float)> func;
    float measures;
    float start;
    float baseline;
    float amplitude;
};

class Sine : public Action {
public:
    Sine(float measures, float start, float baseline, float amplitude);

    float get_value(float measure,
                    float previous_end,
                    const Action *previous,
                    float next_start,
                    const Action *next) const override;
private:
    float measures;
    float start;
    float baseline;
    float amplitude;
};

class Sawtooth : public Action {
public:
    Sawtooth(float measures, float start, float baseline, float amplitude);

    float get_value(float measure,
                    float previous_end,
                    const Action *previous,
                    float next_start,
                    const Action *next) const override;

private:
    float measures;
    float start;
    float baseline;
    float amplitude;
};

class Steady : public Action {
public:
    Steady(float increase_per_measure, float start);

    float get_value(float measure,
                    float previous_end,
                    const Action *previous,
                    float next_start,
                    const Action *next) const override;

private:
    float increase_per_measure;
    float start;

};

std::unique_ptr<Action> create_action(const std::string &name, const std::vector<float> &params);

}
