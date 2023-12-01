#pragma once

#include <string>
#include <iostream>
#include <map>
#include <vector>
#include <set>
#include <algorithm>
#include <numeric>
#include <cmath>
#include <deque>
#include <unordered_map>
#include <unordered_set>
#include <memory>

#include "geo.h"


namespace transport_catalogue {
    using geo::Coordinates;
    struct Stop {
        std::string name;
        Coordinates coordinates;
    };


    struct Bus {
        bool is_looped;
        std::string name;
        std::vector<std::shared_ptr<Stop>> route;
    };
    struct BusInfo {
        int stops_in_route;
        int unique_route;
        double route_length;
        double geography_length;
    };
    
    class TransportCatalogue {

        

        struct StringViewHasher {
            std::size_t operator()(std::string_view str) const {
                return std::hash<std::string_view>{}(str);
            }
        };
        struct HasherStop {
            std::size_t operator()(std::pair<std::shared_ptr<Stop>, std::shared_ptr<Stop> >stop) const {
                return std::hash<const void*>{}(stop.first.get()) + std::hash<const void*>{}(stop.second.get());
            }
        };

    public:

        TransportCatalogue() = default;

        void AddStop(const std::string& name, Coordinates coordinates);

        void AddStopDistance(std::shared_ptr<Stop> stop_first, std::shared_ptr<Stop> stop_second, double distance);

        void AddBus(const std::string& name, const std::vector<std::string>& stop_names, bool is_loop);

        std::shared_ptr<Stop> FindStop(std::string_view stop_name) const;

        std::shared_ptr<Bus> FindBus(const std::string_view name) const;

        BusInfo GetBusInfo(std::string_view bus_name) const;

        std::set<std::string_view> GetStopInfo(std::string_view stop_name) const;

        double GetRealDistance(std::shared_ptr<Stop> from, std::shared_ptr<Stop>to) const;

        std::unordered_map<std::string_view, std::shared_ptr<Bus>, StringViewHasher> GetAllBuses()const;

        std::set<std::string> GetBusName() const ;
         

    private:


        std::deque<Stop> stops_;
        std::deque<Bus> buses_;
        std::set<std::string> busname_;
        std::unordered_map<std::string_view, std::shared_ptr<Stop>, StringViewHasher> stopname_to_stop_;
        std::unordered_map<std::string_view, std::shared_ptr<Bus>, StringViewHasher> busname_to_busese_;
        std::unordered_map<std::string_view, std::set<std::string_view>, StringViewHasher> stopname_to_busese_;
        std::unordered_map<std::pair<std::shared_ptr<Stop>, std::shared_ptr<Stop>>, double, HasherStop> stops_pair_to_distance_;





        bool IsValueName(std::string_view text);

        bool IsValidLatitude(double number);

        bool IsValidLongitude(double number);

        int GetNumberOfStops(const std::string_view name) const;

        int GetNumberOfUniqueStops(const std::string_view name) const;


    };
}
