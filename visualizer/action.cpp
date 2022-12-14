#include "action.h"

#include <SDL_stdinc.h>

#include <cmath>
#include <stdexcept>

namespace visualizer {

Action::Action(float start) : start(start) {  }

namespace {

class Step : public Action {
public:
    Step(float start, float length, const std::vector<float> &values)
      : Action(start),
        length(length / values.size()),
        values(values)
    { }

    float get_value(float measure) const override {
        const auto pos = static_cast<int>((measure - get_start()) / length) % values.size();
        return values[pos];
    }

private:
    float length;
    std::vector<float> values;
};

class ControlPoint {
public:
    ControlPoint(float time, float value)
      : time(time),
        value(value),
        left_derivative(NAN),
        right_derivative(NAN)
    { }

    ControlPoint(float time, float value, float derivative)
      : time(time),
        value(value),
        left_derivative(derivative),
        right_derivative(derivative)
    { }

    ControlPoint(float time, float value, float left_derivative, float right_derivative)
      : time(time),
        value(value),
        left_derivative(left_derivative),
        right_derivative(right_derivative)
    { }

    float time;
    float value;
    float left_derivative;
    float right_derivative;
};

bool operator < (const ControlPoint &a, const ControlPoint &b) {
    return a.time < b.time;
}

bool operator < (float a, const ControlPoint &b) {
    return a < b.time;
}

float calc_derivative(const ControlPoint &a, const ControlPoint &b) {
    return (b.value - a.value) / (b.time - a.time);
}

float newton_interpolation(float a, float fa, float faa, float b, float fb, float fbb, float x) {
    const float fab = (fb - fa) / (b - a);
    const float faab = (fab - faa) / (b - a);
    const float fabb = (fbb - fab) / (b - a);
    const float faabb = (fabb - faab) / (b - a);
    return fa + faa * (x - a) + faab * (x - a) * (x - a) + faabb * (x - a) * (x - a) * (x - b);
}

float newton_interpolation(const ControlPoint &a, const ControlPoint &b, float t) {
    return newton_interpolation(a.time, a.value, a.right_derivative, b.time, b.value, b.left_derivative, t);
}

float modf(float x, float m) {
    return x - std::floor(x / m) * m;
}

template<typename Iter, typename Value>
Iter find_last_less_than_or_equal(Iter begin, Iter end, const Value &value) {
    if (begin == end) {
        return end;
    }
    const Iter result = std::upper_bound(begin, end, value);
    if (result == begin) {
        return end;
    } else {
        return std::prev(result);
    }
}

class HermiteSpline : public Action {
public:
    using ControlPoints = std::vector<ControlPoint>;

    HermiteSpline(float start, ControlPoints control_points)
      : Action(start),
        control_points(control_points)
    {
        HermiteSpline::ControlPoints::iterator previous = this->control_points.begin();
        HermiteSpline::ControlPoints::iterator current = std::next(previous);
        HermiteSpline::ControlPoints::iterator next = std::next(current);

        if (isnan(previous->left_derivative) || isnan(previous->right_derivative)) {
            const float derivative = calc_derivative(*previous, *current);
            previous->left_derivative = derivative;
            previous->right_derivative = derivative;
        }
        while (next != this->control_points.end()) {
            if (isnan(current->left_derivative) || isnan(current->right_derivative)) {
                const float derivative = calc_derivative(*next, *previous);
                current->left_derivative = derivative;
                current->right_derivative = derivative;
            }
            ++previous;
            ++current;
            ++next;
        }
        if (isnan(current->left_derivative) || isnan(current->right_derivative)) {
            const float derivative = calc_derivative(*previous, *current);
            current->left_derivative = derivative;
            current->right_derivative = derivative;
        }
    }

protected:
    float calc_value(float time) const {
        ControlPoints::const_iterator current = find_last_less_than_or_equal(control_points.begin(), control_points.end(), time);
        if (current == control_points.end()) {
            current = control_points.begin();
        } else if (std::next(current) == control_points.end()) {
            --current;
        }
        const ControlPoints::const_iterator next = std::next(current);
        return newton_interpolation(*current, *next, time);
    }

    ControlPoints control_points;
};

HermiteSpline::ControlPoints parse_control_points(const nlohmann::json &control_points) {
    HermiteSpline::ControlPoints result;
    for (const auto &item : control_points.items()) {
        const float time = std::stof(item.key());
        const auto &parameter = item.value();
        if (parameter.is_array()) {
            if (parameter.size() == 1) {
                const float value = parameter.at(0).get<float>();
                result.emplace_back(time, value);
            } else if (parameter.size() == 2) {
                const float value = parameter.at(0).get<float>();
                const float derivative = parameter.at(1).get<float>();
                result.emplace_back(time, value, derivative);
            } else if (parameter.size() == 3) {
                const float value = parameter.at(0).get<float>();
                const float left_derivative = parameter.at(1).get<float>();
                const float right_derivative = parameter.at(2).get<float>();
                result.emplace_back(time, value, left_derivative, right_derivative);
            }
        } else if (parameter.is_number()) {
            result.emplace_back(time, parameter.get<float>());
        }
    }
    std::sort(result.begin(), result.end());
    return result;
}

HermiteSpline::ControlPoints periodic_catmull_rom_spline(float length, const nlohmann::json &control_points) {
    HermiteSpline::ControlPoints result = parse_control_points(control_points);

    ControlPoint front_mirror = result.back();
    front_mirror.time -= length;
    ControlPoint back_mirror = result.front();
    back_mirror.time += length;
    result.insert(result.begin(), front_mirror);
    result.push_back(back_mirror);

    return result;
}

class PeriodicSpline : public HermiteSpline {
public:
    PeriodicSpline(float start, float length, const nlohmann::json &control_points)
      : HermiteSpline(start, periodic_catmull_rom_spline(length, control_points)),
        length(length)
    {
        auto &first = this->control_points.front();
        auto &second = this->control_points[1];
        auto &last = this->control_points.back();
        auto &second_to_last = this->control_points[control_points.size() - 2];
        first.left_derivative = second_to_last.left_derivative;
        first.right_derivative = second_to_last.right_derivative;
        last.left_derivative = second.left_derivative;
        last.right_derivative = second.right_derivative;
    }

    float get_value(float measure) const override {
        return calc_value(modf(measure - get_start(), length));
    }

private:
    float length;
};


class Spline : public HermiteSpline {
public:
    Spline(float start, const nlohmann::json &control_points)
      : HermiteSpline(start, parse_control_points(control_points))
    { }

    float get_value(float measure) const override {
        return calc_value(measure - get_start());
    }
};

}

std::unique_ptr<Action> create_action(float start, const nlohmann::json &action) {
    const std::string &name = action.at("action").get<std::string>();
    const auto &parameters = action.at("parameters");
    if (name == "step") {
        const float length = parameters["length"].get<float>();
        const std::vector<float> params = parameters["values"].get<std::vector<float>>();
        return std::make_unique<Step>(start, length, params);
    } else if (name == "periodic-spline") {
        const float length = parameters["length"].get<float>();
        const auto &control_points = parameters["control-points"];
        return std::make_unique<PeriodicSpline>(start, length, control_points);
    } else if (name == "spline") {
        const auto &control_points = parameters["control-points"];
        return std::make_unique<Spline>(start, control_points);
    } else {
        throw std::runtime_error("Unknown action " + name);
    }
}

}
