#pragma once

namespace visualizer {

class Action {
public:
    Action() = default;
    virtual ~Action() = default;
    virtual float get_value(unsigned long ms,
                            unsigned long previous_end,
                            const Action *previous,
                            unsigned long next_start,
                            const Action *next) const = 0;

    Action(const Action &) = delete;
    Action &operator = (const Action &) = delete;
};

class Constant : public Action {
public:
    Constant(float value);

    float get_value(unsigned long ms,
                    unsigned long previous_end,
                    const Action *previous,
                    unsigned long next_start,
                    const Action *next) const override;

private:
    float value;
};

class Linear : public Action {
public:
    float get_value(unsigned long ms,
                    unsigned long previous_end,
                    const Action *previous,
                    unsigned long next_start,
                    const Action *next) const override;
};

class Smooth : public Action {
public:
    float get_value(unsigned long ms,
                    unsigned long previous_end,
                    const Action *previous,
                    unsigned long next_start,
                    const Action *next) const override;
};

class Sine : public Action {
public:
    Sine(float freq, float phase, float baseline, float amplitude);

    float get_value(unsigned long ms,
                    unsigned long previous_end,
                    const Action *previous,
                    unsigned long next_start,
                    const Action *next) const override;
private:
    float freq;
    float phase;
    float baseline;
    float amplitude;
};

class Sawtooth : public Action {
public:
    Sawtooth(float freq, float phase, float baseline, float amplitude);

    float get_value(unsigned long ms,
                    unsigned long previous_end,
                    const Action *previous,
                    unsigned long next_start,
                    const Action *next) const override;

private:
    float freq;
    float phase;
    float baseline;
    float amplitude;
};

}
