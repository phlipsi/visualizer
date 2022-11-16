#pragma once

namespace visualizer {

class Transition {
public:
    Transition(float start, float end);

    float get_start() const { return start; }
    float get_end() const { return end; }

private:
    float start;
    float end;
};

inline bool operator < (float measure, const Transition &b) {
    return measure < b.get_start();
}


inline bool operator < (const Transition &a, const Transition &b) {
    return a.get_start() < b.get_start();
}

}
