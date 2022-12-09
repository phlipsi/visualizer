#include "parameters.h"

#include <SDL_stdinc.h>
#include <nlohmann/json.hpp>

#include <imgui.h>
#include <implot.h>

#include <cmath>
#include <stdexcept>

namespace visualizer {

Parameters::Parameters(const std::string &filename) : debugged(parameters.end()) {
    load(filename);
}

void Parameters::load(const std::string &filename) {
    clear();

    std::ifstream input(filename);
    const auto choreography = nlohmann::json::parse(input);
    const auto general = choreography["general"];
    const float bpm = general["bpm"].get<float>();
    const std::string meter = general["meter"].get<std::string>();
    const std::string::size_type slash = meter.find('/');
    int meter_num = 0;
    int meter_denum = 0;
    if (slash != std::string::npos) {
        meter_num = std::stoi(meter.substr(0, slash));
        meter_denum = std::stoi(meter.substr(slash + 1));
    }
    ms_per_measure = static_cast<float>(meter_num) * 60000.0f / bpm;

    const auto parameters = choreography["parameters"];
    for (const auto &entry : parameters.items()) {
        const std::string name = entry.key();
        for (const auto &timestamp : entry.value().items()) {
            const float measure = std::stof(timestamp.key());
            const auto &values = timestamp.value();
            if (values.contains("transition")) {
                this->parameters[name].add_transition(create_transition(measure, values["transition"]));
            }
            this->parameters[name].add_action(create_action(measure, values["action"]));
        }
    }
}

void Parameters::clear() {
    debugged = parameters.end();
    for (auto &entry : parameters) {
        entry.second.clear();
    }
}

void Parameters::set_measure(float measure) {
    const bool debugging = debug_output.is_open();
    if (debugging) {
        debug_output << measure;
    }
    for (auto &entry : parameters) {
        entry.second.set_measure(measure);
        if (debugging) {
            debug_output << ';' << entry.second.get_value();
        }
    }
    if (debugging) {
        debug_output << '\n';
    }
}

void Parameters::set_debug_output(const std::string &filename) {
    debug_output.close();
    debug_output.open(filename);
    debug_output << "measure";
    for (const auto &entry : parameters) {
        debug_output << ';' << entry.first;
    }
    debug_output << '\n';
}

const float &Parameters::get_parameter(const std::string &name) {
    const auto it = parameters.find(name);
    if (it == parameters.end()) {
        throw std::runtime_error("Undefined parameter " + name);
    }
    return it->second.get_value();
}

void Parameters::choose_debugged_parameter() {
    const char *name = "";
    if (debugged != parameters.end()) {
        name = debugged->first.c_str();
    }
    if (ImGui::BeginCombo("Parameters", name)) {
        for (auto it = parameters.begin(); it != parameters.end(); ++it) {
            const bool is_selected = it == debugged;
            if (ImGui::Selectable(it->first.c_str(), is_selected)) {
                debugged = it;
            }
            if (is_selected) {
                ImGui::SetItemDefaultFocus();
            }
        }
        ImGui::EndCombo();
    }
}

namespace {

class Plotter {
public:
    Plotter(Parameter &parameter, float measure, float around, int count)
      : parameter(parameter),
        xstep(2.0f * around / static_cast<float>(count)),
        measure(measure),
        around(around) { }

    static ImPlotPoint call(int idx, void *data) {
        Plotter *self = static_cast<Plotter *>(data);
        return self->call_me(idx);
    }

    ImPlotPoint call_me(int idx) {
        const float xmin = measure - around;
        const float x = xmin + idx * xstep;
        parameter.set_measure(x >= 0.0f ? x : 0.0f);
        return ImPlotPoint(x, parameter.get_value());
    }

private:
    Parameter &parameter;
    float xstep;
    float measure;
    float around;
};

}

void Parameters::plot_debugger_parameter(float measure, float around, int count) {
    if (debugged != parameters.end()) {
        Plotter plotter(debugged->second, measure, around, count);
        ImPlot::PlotLineG(debugged->first.c_str(), &Plotter::call, &plotter, 1000);
    }
}

}
