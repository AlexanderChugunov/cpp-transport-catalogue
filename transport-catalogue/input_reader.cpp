#include "input_reader.h"
#include "stat_reader.h"

#include <algorithm>
#include <unordered_map>

using namespace std;
int QUERY_COUNT;
namespace transport_catalogue {

namespace query {

inline std::vector<std::string_view> Split(std::string_view command, char c) {
    std::vector<std::string_view> result;
    int pos = 0;
    const int pos_end = command.npos;

    while (true) {
        int distance = command.find(c, pos);
        result.push_back(distance == pos_end ? command.substr(pos) : command.substr(pos, distance - pos));

        if (distance == pos_end) {
            break;
        } else {
            pos = distance + 1;
        }
    }

    return result;
}

pair<string_view, string_view> Command::ParseCoordinates(string_view latitude, string_view longitude) {
    while (longitude.front() == ' ') {
        longitude.remove_prefix(1);
    }

    while (latitude.front() == ' ') {
        latitude.remove_prefix(1);
    }

    while (longitude.back() == ' ') {
        longitude.remove_suffix(1);
    }

    while (latitude.back() == ' ') {
        latitude.remove_suffix(1);
    }

    return make_pair(latitude, longitude);
}

vector<pair<string_view, string_view>> Command::ParseDistances(const vector<string_view> const_vec_input) {
    vector<pair<string_view, string_view>> result;
    size_t i = 2;
    vector<string_view> vec_input = const_vec_input;
    while (i < vec_input.size()) {
        while (vec_input[i].front() == ' ') {
            vec_input[i].remove_prefix(1);
        }

        auto pos_m = vec_input[i].find('m');
        string_view dist, stop;
        dist = vec_input[i].substr(0, pos_m);
        auto pos_t = vec_input[i].find('t');
        pos_t += 2;
        stop = vec_input[i].substr(pos_t, vec_input[i].length());

        while (stop.front() == ' ') {
            stop.remove_prefix(1);
        }

        while (stop.back() == ' ') {
            stop.remove_suffix(1);
        }

        result.push_back({dist, stop});
        ++i;
    }
    return result;
}

vector<string_view> Command::ParseBuses(const vector<string_view> vec_input) {
    vector<string_view> result;
    vector<string_view> parsed_buses;

    if (vec_input.size() > 3) {
        if (desc_command.find('-') != string::npos) {
            route_type = RouteType::Direct;
            parsed_buses = Split(desc_command, '-');
        }

        if (desc_command.find('>') != string::npos) {
            route_type = RouteType::Round;
            parsed_buses = Split(desc_command, '>');
        }
    }
    for (size_t i = 0; i < parsed_buses.size(); ++i) {
        while (parsed_buses[i].front() == ' ') {
            parsed_buses[i].remove_prefix(1);
        }

        while (parsed_buses[i].back() == ' ') {
            parsed_buses[i].remove_suffix(1);
        }

        result.push_back(parsed_buses[i]);
    }

    return result;
}

void Command::ParseCommandString(const string input) {
    static std::unordered_map<std::string, QueryType> const table = {
        {"Stop", QueryType::StopX}, {"Bus", QueryType::BusX}
    };
    origin_command = move(input);
    auto vec_input = Split(origin_command, ' ');
    auto start = origin_command.find_first_not_of(' ');
    auto end = origin_command.find(' ', start);
    string temp_type = {origin_command.begin() + start, origin_command.begin() + end};

    switch (table.at(temp_type)) {
        case QueryType::StopX:
            type = QueryType::StopX;
            if (auto pos = origin_command.find(':'); pos != string::npos) {
                
                desc_command = origin_command.substr(pos + 2, origin_command.length() - pos - 1);
                auto temp = Split(desc_command, ',');

                for (size_t i = end; i < pos; ++i) {
                    name += origin_command[i];
                }

                while (name.front() == ' ') {
                    name.erase(name.begin());
                }

                coordinates = ParseCoordinates(temp[0], temp[1]);

                if (temp.size() > 2) {
                    distances = ParseDistances(temp);
                }
            }
            else {
                for (size_t i = end; i < origin_command.size(); ++i) {
                    name += origin_command[i];
                }

                while (name.front() == ' ') {
                    name.erase(name.begin());
                }

                while (name.back() == ' ') {
                    name.erase(name.end() - 1);
                }
                break;
            }
            break;
        case QueryType::BusX:
            type = QueryType::BusX;
            if (auto pos = origin_command.find(':'); pos != string::npos) {
                desc_command = origin_command.substr(pos + 2, origin_command.length() - pos - 1);           
                for (size_t i = end; i < pos; ++i) {
                    name += origin_command[i];
                }

                while (name.front() == ' ') {
                    name.erase(name.begin());
                }
                route = ParseBuses(vec_input);
            }
            else {
                for (size_t i = end; i < origin_command.size(); ++i) {
                    name += origin_command[i];
                }

                while (name.front() == ' ') {
                    name.erase(name.begin());
                }

                while (name.back() == ' ') {
                    name.erase(name.end() - 1);
                }
                break;
            }
            break;
    }
}

void InputReader::ParseInput() {
    
    cin >> QUERY_COUNT;
    cin.ignore();
    string command;

    for (int i = 0; i < QUERY_COUNT; ++i) {
        getline(cin, command);
        Command cur_command;
        cur_command.ParseCommandString(move(command));
        commands_.push_back(move(cur_command));
    }
}

void InputReader::Load(TransportCatalogue& tc) {
    auto it_desc = partition(commands_.begin(), commands_.end(), [](Command com) {
        return !com.desc_command.empty();
    });
    auto it_stops = partition(commands_.begin(), it_desc, [](Command com) {
        return com.type == QueryType::StopX;
    });

    for (auto cur_it = commands_.begin(); cur_it != it_stops; ++cur_it) {
        InputReader::LoadCommand(tc, *cur_it, 0);
    }
    for (auto cur_it = commands_.begin(); cur_it != it_stops; ++cur_it) {
        InputReader::LoadCommand(tc, *cur_it, 1);
    }
    for (auto cur_it = it_stops; cur_it != it_desc; ++cur_it) {
        InputReader::LoadCommand(tc, *cur_it, 0);
    }
    for (auto cur_it = it_desc; cur_it != commands_.end(); ++cur_it) {
        InputReader::LoadCommand(tc, *cur_it, 0);
    }
}

void InputReader::LoadCommand(TransportCatalogue& tc, Command com, bool dist) {
    switch (com.type) {
        case QueryType::StopX:
            if (com.coordinates != pair<string_view, string_view>()) {
                string lat = {com.coordinates.first.begin(), com.coordinates.first.end()};
                string lon = {com.coordinates.second.begin(), com.coordinates.second.end()};

                if (dist == false)  {
                    tc.AddStop(com.name, stod(lat), stod(lon));
                }
                else {
                    if (!com.distances.empty()) {
                        for (auto& [dist, stop] : com.distances) {
                            string dist_str = {dist.begin(), dist.end()};
                            tc.SetStopDistance(com.name, stoull(dist_str), stop);
                        }
                    }
                }
            }
            else {
                output::OutputStopAbout(tc, com.name);
            }

            break;
        case QueryType::BusX:
            if (!com.route.empty()) {
                tc.AddBus(com.name, com.route_type, com.route);
            }
            else {
                output::OutputRouteAbout(tc, com.name);
            }
            break;
    }
}

}
}
