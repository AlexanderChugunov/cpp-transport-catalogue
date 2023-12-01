#include "request_handler.h"


RequestHandler::RequestHandler( transport_catalogue::TransportCatalogue& db,  renderer::MapRenderer& renderer):
	db_(db),
	renderer_(renderer)
{
}

// Возвращает информацию о маршруте (запрос Bus)
std::shared_ptr<transport_catalogue::Bus> RequestHandler::GetBusStat(const std::string_view& bus_name) const {
	return db_.FindBus(bus_name);
}

// Возвращает маршруты, проходящие через
 std::set<std::string_view> RequestHandler::GetBusesByStop(const std::string_view& stop_name) const {
	return db_.GetStopInfo(stop_name);
}



svg::Document RequestHandler::RenderMap() const {
	return renderer_.GetDocument();
}


void RequestHandler::BuildMap() {
	
	auto name_ = db_.GetBusName();
	
	std::set<std::string> stop_name;
	
	if (!name_.empty()) {
		std::vector <geo::Coordinates> all_coordinates;
		std::vector <std::vector<geo::Coordinates>> coordinates;
		std::map<std::string, geo::Coordinates> stopname_to_cooord;
		for (auto& name : name_) {
			auto bus = db_.FindBus(name);
			if (!bus.get()->route.empty()) {
				std::vector <geo::Coordinates> route_coordinates;

				for (auto stop_ : bus.get()->route) {

					route_coordinates.push_back(stop_.get()->coordinates);
					all_coordinates.push_back(stop_.get()->coordinates);
					if (stopname_to_cooord.find(stop_.get()->name) == stopname_to_cooord.end()) {
						stopname_to_cooord[stop_.get()->name] = stop_.get()->coordinates;
					}
				}
				coordinates.push_back(route_coordinates);
			}
		}


		renderer::SphereProjector proj_ = renderer_.AddRoute(all_coordinates, coordinates);





		BuildBusName(proj_);
		BuildStops(stopname_to_cooord, proj_);
	}

}
void RequestHandler::BuildBusName(renderer::SphereProjector proj_) {
	auto names = db_.GetBusName();
	if (!names.empty()) {
		int start = 0;
		for (auto name : names) {
			
			auto bus = db_.FindBus(name);
			if (!bus.get()->route.empty()) {
				if (bus.get()->is_looped) {
					renderer_.AddBusName(bus.get()->name, bus.get()->route[0].get()->coordinates, bus.get()->route[0].get()->coordinates, proj_,start);
				}
				else {
					int size_ = bus.get()->route.size();
					renderer_.AddBusName(bus.get()->name, bus.get()->route[0].get()->coordinates, bus.get()->route[size_/2].get()->coordinates, proj_,start);
				}
				start++;
			}
		}
	}
	
}

void RequestHandler::BuildStops(std::map<std::string, geo::Coordinates>& stops, renderer::SphereProjector proj_) {
	renderer_.AddStop(stops, proj_);

}
