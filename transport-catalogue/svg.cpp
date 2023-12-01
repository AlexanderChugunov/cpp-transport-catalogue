#include "svg.h"

namespace svg {

	using namespace std::literals;

	void Object::Render(const RenderContext& context) const {
		context.RenderIndent();

		// Делегируем вывод тега своим подклассам
		RenderObject(context);

		context.out << std::endl;
	}

	// ---------- Circle ------------------

	Circle& Circle::SetCenter(Point center) {
		center_ = center;
		return *this;
	}

	Circle& Circle::SetRadius(double radius) {
		radius_ = radius;
		return *this;
	}

	void Circle::RenderObject(const RenderContext& context) const {
		auto& out = context.out;
		out << "<circle cx=\""sv << center_.x << "\" cy=\""sv << center_.y << "\" "sv;
		out << "r=\""sv << radius_ << "\""sv;
		RenderAttrs(out);
		out << "/>"sv;
	}

	// ---------- Polyline ------------------

	Polyline& Polyline::AddPoint(Point point) {
		points_.push_back(std::move(point));
		return *this;
	}

	void Polyline::RenderObject(const RenderContext& context) const {
		auto& out = context.out;
		out << "<polyline points=\""sv;
		for (size_t i = 0; i < points_.size(); ++i) {
			if (i != 0) {
				out << " "sv;
			}
			const Point& point = points_.at(i);
			out << point.x << ","sv << point.y;
		}
		out << "\""sv;
		RenderAttrs(out);
		out << "/>"sv;
	}

	// ---------- Text ------------------

	Text& Text::SetPosition(Point pos) {
		pos_ = pos;
		return *this;
	}

	Text& Text::SetOffset(Point offset) {
		offset_ = offset;
		return *this;
	}

	Text& Text::SetFontSize(uint32_t size) {
		font_size_ = size;
		return *this;
	}

	Text& Text::SetFontFamily(const std::string& font_family) {
		font_family_ = font_family;
		return *this;
	}

	Text& Text::SetFontWeight(const std::string& font_weight) {
		font_weight_ = font_weight;
		return *this;
	}

	Text& Text::SetData(const std::string& data) {
		data_ = data;
		return *this;
	}

	void Text::PrintText(const RenderContext& context) const {
		auto& out = context.out;
		for (char c : data_) {
			switch (c) {
			case '\"':
				out << "&quot;";
				break;
			case '\'':
				out << "&apos;";
				break;
			case '<':
				out << "&lt;";
				break;
			case '>':
				out << "&gt;";
				break;
			case '&':
				out << "&amp;";
				break;
			default:
				out << c;
			}
		}
	}

	void Text::RenderObject(const RenderContext& context) const {
		auto& out = context.out;
		out << "<text";
		RenderAttrs(out);
		out << " x=\"" << pos_.x << "\"";
		out << " y=\"" << pos_.y << "\"";
		out << " dx=\"" << offset_.x << "\"";
		out << " dy=\"" << offset_.y << "\"";
		out << " font-size=\"" << font_size_ << "\"";
		if (font_family_ != "") {
			out << " font-family=\"" << font_family_ << "\"";
		}
		if (font_weight_ != "") {
			out << " font-weight=\"" << font_weight_ << "\"";
		}
		out << ">";
		Text::PrintText(context);
		out << "</text>";
	}

	// ---------- Document ------------------

	void Document::AddPtr(std::unique_ptr<Object>&& obj) {
		items_.emplace_back(std::move(obj));

	}

	void Document::Render(std::ostream& out) const {
		RenderContext render_context(out);

		out << "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>"sv << std::endl;
		out << "<svg xmlns=\"http://www.w3.org/2000/svg\" version=\"1.1\">"sv << std::endl;

		for (const auto &ptr : items_) {
			out << "  ";
			ptr.get()->Render(render_context);
		}

		out << "</svg>"sv;
	}
	size_t Document::Size() {
		return items_.size();
	}

	void Document::Clear() {
		items_.clear();
	}

}  // namespace svg
