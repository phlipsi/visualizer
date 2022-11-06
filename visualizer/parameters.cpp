#include "parameters.h"

#include <cmath>
#include <stdexcept>

namespace visualizer {

/*namespace {

std::vector<std::string> split(const std::string &s, char sep) {
    std::vector<std::string> result;
    std::string::size_type pos = 0;
    while (pos < s.length()) {
        const std::string::size_type next = s.find(sep, pos);
        if (next == std::string::npos) {
            result.push_back(s.substr(pos));
            break;
        } else {
            result.push_back(s.substr(pos, next - pos);
            pos = next + 1;
        }
    }
    return result;
}

std::string trim(const std::string &s) {
    if (s.empty()) {
        return s;
    }

    const auto start = s.find_first_not_of(" ");
    auto end = s.length() - 1;
    while (s[end] != ' ') && end > start) {
        --end;
    }
    return s.substr(start, end - start);
}

}*/

Parameters::Parameters(std::istream &input) {
    std::map<std::string::size_type, float> measure_positions;
    std::string line;
    unsigned int bpm = 0;
    unsigned int meter_num = 0;
    unsigned int meter_denum = 0;
    while (std::getline(input, line) && !line.empty()) {
        if (line[0] == '#') {
            continue;
        }
        const std::string::size_type sep = line.find(":");
        if (sep != std::string::npos) {
            const std::string key = line.substr(0, sep);
            const std::string value = line.substr(sep + 1);
            if (key == "bpm") {
                bpm = std::stoi(value);
            } else if (key == "meter") {
                const std::string::size_type slash = value.find('/');
                if (slash != std::string::npos) {
                    meter_num = std::stoi(value.substr(0, slash));
                    meter_denum = std::stoi(value.substr(slash + 1));
                }
            }
        }
    }
    // 1 / bpm = mpb (minutes per beat)
    // mpb * 60000 = mspb (milliseconds per beat)
    // meter_num * spb = mspm (milliseconds per measure)
    //ms_per_beat = static_cast<float>(meter_num) * 60000.0f / static_cast<float>(bpm);
    ms_per_measure = static_cast<float>(meter_num) * 60000.0f / static_cast<float>(bpm);

    std::getline(input, line);
    std::string::size_type pos = 4;
    while (pos < line.length()) {
        if (line[pos] != ' ') {
            std::string::size_type end = line.find(" ", pos);
            std::string::size_type len = end;
            if (len != std::string::npos) {
                len = end - pos;
            }
            measure_positions[pos] = std::stof(line.substr(pos, len));
            pos = end;
        } else {
            ++pos;
        }
    }
    while (std::getline(input, line)) {
        if (line.empty() || line[0] == '#') {
            continue;
        }
        const std::string::size_type name_end = line.find(" ");
        const std::string name = line.substr(0, name_end);
        for (const auto &entry : measure_positions) {
            if (entry.first >= line.length()) {
                break;
            } else if (line[entry.first] == ' ') {
                continue;
            }
            const std::string::size_type action_end = line.find_first_of(" ,", entry.first);
            const std::string action = line.substr(entry.first, action_end - entry.first);
            std::vector<float> params;
            if (line[action_end] == ',') {
                std::string::size_type param_pos = action_end + 1;
                while (param_pos < line.length() && line[param_pos] != ' ') {
                    const std::string::size_type param_end = line.find_first_of(" ,", param_pos);
                    std::string::size_type param_len = param_end;
                    if (param_len != std::string::npos) {
                        param_len = param_end - param_pos;
                    }
                    const std::string param = line.substr(param_pos, param_len);
                    if (param == "pi") {
                        params.push_back(static_cast<float>(M_PI));
                    } else if (param == "2pi") {
                        params.push_back(static_cast<float>(M_PI));
                    } else {
                        params.push_back(std::stof(param));
                    }
                    if (param_end == std::string::npos) {
                        break;
                    } else {
                        param_pos = param_end + 1;
                    }
                }
            }
            parameters[name].add_action(entry.second, create_action(action, params));
        }
    }
}

void Parameters::set_measure(float measure) {
    for (auto &entry : parameters) {
        entry.second.set_measure(measure);
    }
}

void Parameters::add_action(const std::string &name, float measure, std::unique_ptr<Action> action) {
    parameters[name].add_action(measure, std::move(action));
}

const float &Parameters::get_parameter(const std::string &name) {
    const auto it = parameters.find(name);
    if (it == parameters.end()) {
        throw std::runtime_error("Undefined parameter");
    }
    return it->second.get_value();
}

}
