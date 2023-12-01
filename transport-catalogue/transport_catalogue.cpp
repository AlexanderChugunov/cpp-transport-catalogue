#include "transport_catalogue.h"

namespace transport_catalogue {

    void TransportCatalogue::AddStop(const std::string& name, Coordinates coordinates) {
        using namespace std::string_literals;
        if (!IsValidLatitude(coordinates.lat)) {
            throw std::invalid_argument("latitude invvalue"s);
        }
        if (!IsValidLongitude(coordinates.lng)) {
            throw std::invalid_argument("longitude invvalue"s);
        }/*
        if (!IsValueName(name)) {
            throw std::invalid_argument("Stop name invvalue"s);
        }*/
        if (!FindStop(name)) {
            stops_.emplace_back(Stop{name, coordinates });
            auto stop_ptr = std::make_shared<Stop>(stops_.back());

            stopname_to_stop_[stop_ptr->name] = stop_ptr;;
        }



    }

    void TransportCatalogue::AddStopDistance(std::shared_ptr<Stop> stop_first, std::shared_ptr<Stop> stop_second, double distance) {
        stops_pair_to_distance_[{stop_first, stop_second}] = distance;
    }

    void TransportCatalogue::AddBus(const std::string& name, const std::vector<std::string>& stop_names, bool is_loop) {
        using namespace std::string_literals;
        //if (!IsValueName(name)) {
          //  throw std::invalid_argument("Bus name contains prohibited characters"s);
        //}
        std::vector<std::shared_ptr<Stop>> stops;
        for (auto& stop : stop_names) {
            auto st = FindStop(stop);
            if (!st) {
                throw std::out_of_range("The stop is not in the catalog"s);
            }
            stops.push_back(st);

        }
        if (!is_loop) {
            for (int it = static_cast<int>(stop_names.size()) - 2; it != -1; it--) {
                auto st = FindStop(stop_names[it]);
                stops.push_back(st);
            }
        }
        busname_.insert(name);
        Bus bus_ = { is_loop, name, std::move(stops) };
        buses_.emplace_back(std::move(bus_));
        auto bus1 = std::make_shared<Bus>(buses_.back());
        for (auto stop : bus1->route) {
            stopname_to_busese_[stop->name].insert(std::move(bus1->name));

        }
        busname_to_busese_[bus1->name] = bus1;
    }

    std::shared_ptr<Stop> TransportCatalogue::FindStop(std::string_view stop_name) const {
        return (stopname_to_stop_.find(stop_name) != stopname_to_stop_.end()) ? stopname_to_stop_.at(stop_name) : nullptr;
    }

    std::shared_ptr<Bus> TransportCatalogue::FindBus(const std::string_view name) const {
        return (busname_to_busese_.find(name) != busname_to_busese_.end()) ? busname_to_busese_.at(name) : nullptr;
    }

    BusInfo TransportCatalogue::GetBusInfo(std::string_view bus_name) const {
        if (busname_to_busese_.find(bus_name) == busname_to_busese_.end()) {
            throw(std::invalid_argument("not found"));
        }
        auto bus = busname_to_busese_.at(bus_name);

        int size = GetNumberOfStops(bus_name);
        int u_size = GetNumberOfUniqueStops(bus_name);
        double real_length = 0;
        double geography_length = 0;

        for (int i = 1; i < static_cast<int>(bus->route.size()); ++i) {
            auto first = bus->route[i - 1];
            auto sec = bus->route[i];
            real_length += GetRealDistance(first, sec);
            geography_length += ComputeDistance(first->coordinates, sec->coordinates);
        }

        return { size , u_size , real_length, real_length / geography_length }; 
    }

    std::set<std::string_view> TransportCatalogue::GetStopInfo(std::string_view stop_name) const {
        std::set<std::string_view> tmp_;
        if (stopname_to_stop_.find(stop_name) == stopname_to_stop_.end()) {
            throw(std::invalid_argument("not found"));
        }
        else if (stopname_to_busese_.find(stop_name) == stopname_to_busese_.end()) {
            return tmp_;
        }
        return stopname_to_busese_.at(stop_name);
    }


    double TransportCatalogue::GetRealDistance(std::shared_ptr<Stop> from, std::shared_ptr<Stop> to) const {
        if (stops_pair_to_distance_.find({ from, to }) == stops_pair_to_distance_.end()) {
            return stops_pair_to_distance_.at({ to, from });
        }
        return stops_pair_to_distance_.at({ from, to });
    }


    std::unordered_map<std::string_view, std::shared_ptr<Bus>, TransportCatalogue::StringViewHasher> TransportCatalogue::GetAllBuses()const {
        return busname_to_busese_;
    }


    std::set<std::string> TransportCatalogue::GetBusName() const {
        return busname_;
    }
    bool TransportCatalogue::IsValueName(std::string_view text) {
        return 	std::any_of(text.begin(), text.end(), [](char c) {
            return isalnum(c);
            });;
    }

    bool TransportCatalogue::IsValidLatitude(double number) {
        return -90.0 <= number && number <= 90.0;
    }

    bool TransportCatalogue::IsValidLongitude(double number) {
        return -180.0 <= number && number <= 180.0;
    }

    int TransportCatalogue::GetNumberOfStops(const std::string_view name) const {
        const auto bus = FindBus(name);
        return static_cast<int>(bus->route.size());
    }

    int TransportCatalogue::GetNumberOfUniqueStops(const std::string_view name) const {
        const auto bus = FindBus(name);
        std::unordered_set<std::string_view> result;
        for (auto stop : bus->route) {
            result.insert(stop->name);
        }
        return static_cast<int>(result.size());
    }

}
