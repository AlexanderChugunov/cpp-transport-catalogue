#include "map_renderer.h"







namespace renderer {

   
    inline bool IsZero(double value) {
        return std::abs(value) < EPSILON;
    }













    MapRenderer::MapRenderer(RenderSettings setting) :
        setting_(setting)

    {

    }
    void MapRenderer::SetSetting(RenderSettings setting) {
        setting_ = setting;
    }

    SphereProjector MapRenderer::AddRoute(std::vector<geo::Coordinates> coord, std::vector< std::vector<geo::Coordinates>> all_stop) {
        auto proj = AddAllRoute(coord);
        
        int start = 0;
        for (auto route : all_stop) {



            svg::Polyline line_;
            line_.SetFillColor(svg::NoneColor);
            line_.SetStrokeColor(setting_.color_palette[start % setting_.color_palette.size()]) ;
            line_.SetStrokeWidth(setting_.line_width);
            line_.SetStrokeLineJoin(svg::StrokeLineJoin::ROUND);
            line_.SetStrokeLineCap(svg::StrokeLineCap::ROUND);
            //SphereProjector proj(coord.begin(), coord.end(), setting_.width, setting_.height, setting_.padding);
            start++;
            for (auto coord_ : route) {
                auto point = proj(coord_);
                line_.AddPoint(point);
               
                //pnt_.SetCenter(point);
                //pnt_.SetRadius(setting_.stop_radius);
            }
            doc_.Add(line_);
        }


        return proj;
    }
    void MapRenderer::AddBusName(std::string bus_name, geo::Coordinates begin, geo::Coordinates end, SphereProjector& proj,int start) {
        bool flag = (begin != end ? true : false);

        svg::Text text;
        svg::Text under_text;

        
        text.SetFillColor(setting_.color_palette[start % setting_.color_palette.size()]);

        text.SetPosition(proj(begin));
        under_text.SetPosition(proj(begin));
        text.SetOffset(svg::Point(setting_.bus_label_offset[0], setting_.bus_label_offset[1]));
        under_text.SetOffset(svg::Point(setting_.bus_label_offset[0], setting_.bus_label_offset[1]));
        text.SetFontSize(setting_.bus_label_font_size);
        under_text.SetFontSize(setting_.bus_label_font_size);
        text.SetFontFamily("Verdana");
        under_text.SetFontFamily("Verdana");
        text.SetFontWeight("bold");
        under_text.SetFontWeight("bold");
        text.SetData(bus_name);
        under_text.SetData(bus_name);

        under_text.SetStrokeWidth(setting_.underlayer_width);
        under_text.SetFillColor(setting_.underlayer_color);
        under_text.SetStrokeColor(setting_.underlayer_color);
        under_text.SetStrokeLineCap(svg::StrokeLineCap::ROUND);
        under_text.SetStrokeLineJoin(svg::StrokeLineJoin::ROUND);

        doc_.Add(under_text);
        doc_.Add(text);
        
        if (flag) {
            svg::Text text_end  = text;
            svg::Text under_text_end = under_text;
            text_end.SetPosition(proj(end));
            under_text_end.SetPosition(proj(end));
            doc_.Add(under_text_end);
            doc_.Add(text_end);
            
        }

    }

    void MapRenderer::AddStop(std::map<std::string, geo::Coordinates>& stops, SphereProjector& proj) {
        for (const auto& [_, cord]: stops) {
            svg::Circle stop_point_;
            stop_point_.SetCenter(proj(cord));
            stop_point_.SetRadius(setting_.stop_radius);
            stop_point_.SetFillColor("white");
            doc_.Add(stop_point_);
        }   

        AddStopName(stops, proj);
    }

    void MapRenderer::AddStopName(std::map<std::string, geo::Coordinates>&stops, SphereProjector& proj) {
        for (const auto& [name, cord] : stops) {
            svg::Text text;
            svg::Text under_text;


            text.SetFillColor("black");

            text.SetPosition(proj(cord));
            under_text.SetPosition(proj(cord));
            text.SetOffset(svg::Point(setting_.stop_label_offset[0], setting_.stop_label_offset[1]));
            under_text.SetOffset(svg::Point(setting_.stop_label_offset[0], setting_.stop_label_offset[1]));
            text.SetFontSize(setting_.stop_label_font_size);
            under_text.SetFontSize(setting_.stop_label_font_size);
            text.SetFontFamily("Verdana");
            under_text.SetFontFamily("Verdana");
            
            text.SetData(name);
            under_text.SetData(name);

            under_text.SetStrokeWidth(setting_.underlayer_width);
            under_text.SetFillColor(setting_.underlayer_color);
            under_text.SetStrokeColor(setting_.underlayer_color);
            under_text.SetStrokeLineCap(svg::StrokeLineCap::ROUND);
            under_text.SetStrokeLineJoin(svg::StrokeLineJoin::ROUND);

            doc_.Add(under_text);
            doc_.Add(text);
        
        }
    
    
    }

    svg::Document MapRenderer::GetDocument() const {
        return doc_;
    }








    SphereProjector MapRenderer::AddAllRoute(std::vector<geo::Coordinates> all_route_) {
        return SphereProjector(all_route_.begin(), all_route_.end(), setting_.width, setting_.height, setting_.padding);

    }

   



}
