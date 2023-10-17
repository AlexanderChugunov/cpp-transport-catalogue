#include "stat_reader.h"

#include <iomanip>
#include <iostream>
#include <set>

using namespace std;

namespace transport_catalogue {

    namespace output {

        void OutputRouteAbout(TransportCatalogue& tc, std::string_view route, std::ostream& output) {
            if (tc.FindBus(route) == nullptr) {
                output << "Bus "s << route << ": not found"s << endl;
            }
            else {
                BusStat stat = tc.GetStatistics(tc.FindBus(route));

                output << "Bus "s << route << ": "s << stat.number_of_stops
                    << " stops on route, "s << stat.unique_stops
                    << " unique stops, "s << std::setprecision(6)
                    << stat.real_distance << " route length, "s
                    << std::setprecision(6) << stat.curvature
                    << " curvature"s << endl;
            }
        }

        void OutputStopAbout(TransportCatalogue& tc, string_view name, std::ostream& output) {
            bool flag = tc.FindStop(name) != nullptr;
            set<string_view> buses = tc.GetBusInfo(name);

            if (flag) {
                if (buses.size() == 0) {
                    output << "Stop "s << name << ": no buses"s << endl;
                }
                else {
                    output << "Stop "s << name << ": buses "s;
                    for (auto it = buses.begin(); it != buses.end(); ++it) {
                        if (next(it) != buses.end()) {
                            output << (*it) << " "s;
                        }
                        else {
                            output << (*it);
                        }
                    }
                    output << endl;
                }
            }
            else {
                output << "Stop "s << name << ": not found"s << endl;
            }
        }

        void OutputAbout(TransportCatalogue& tc, query::Command com, std::ostream& output) {
            if (com.type == query::QueryType::StopX) {
                OutputStopAbout(tc, com.name, output);
            }

            if (com.type == query::QueryType::BusX) {
                OutputRouteAbout(tc, com.name, output);
            }
        }

    }
}
