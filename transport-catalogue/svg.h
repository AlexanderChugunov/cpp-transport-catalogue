	#pragma once

	#define _USE_MATH_DEFINES

	#include <cstdint>
	#include <iostream>
	#include <memory>
	#include <string>
	#include <variant>
	#include <vector>
	#include <cmath>
	#include <optional>
	#include <iomanip>

	namespace svg {


		struct Rgb {
			Rgb() = default;

			Rgb(uint8_t red, uint8_t green, uint8_t blue)
				: red(red), green(green), blue(blue) {

			}

			uint8_t red = 0;
			uint8_t green = 0;
			uint8_t blue = 0;
		};

		struct Rgba {
			Rgba() = default;

			Rgba(uint8_t red, uint8_t green, uint8_t blue, double opacity)
				: red(red), green(green), blue(blue), opacity(opacity) {

			}

			uint8_t red = 0;
			uint8_t green = 0;
			uint8_t blue = 0;
			double opacity = 1.0;
		};

		using Color = std::variant<std::monostate, Rgb, Rgba, std::string>;
		inline const Color NoneColor{ "none" };

		struct ColorPrinter {
			void operator()(std::monostate) {}

			void operator()(Rgb rgb) {
				using namespace std::literals;

				out << "rgb("sv;
				out << std::to_string(rgb.red) << ","sv;
				out << std::to_string(rgb.green) << ","sv;
				out << std::to_string(rgb.blue) << ")"sv;
			}

			void operator()(Rgba rgba) {
				using namespace std::literals;

				out << "rgba("sv;
				out << std::to_string(rgba.red) << ","sv;
				out << std::to_string(rgba.green) << ","sv;
				out << std::to_string(rgba.blue) << ","sv;
				out << rgba.opacity << ")"sv;
			}

			void operator()(std::string text) {
				out << text;
			}

			std::ostream& out;
		};

		enum class StrokeLineCap {
			BUTT,
			ROUND,
			SQUARE,
		};

		enum class StrokeLineJoin {
			ARCS,
			BEVEL,
			MITER,
			MITER_CLIP,
			ROUND,
		};

		inline std::ostream& operator<<(std::ostream& out, Color color) {
			using namespace std::literals;
			std::visit(ColorPrinter{ out }, color);
			return out;
		}

		inline std::ostream& operator<<(std::ostream& out, StrokeLineCap line_cap_) {
			using namespace std::literals;
			switch (static_cast<int>(line_cap_)) {
			case 0:
				out << "butt"sv;
				break;
			case 1:
				out << "round"sv;
				break;
			case 2:
				out << "square"sv;
				break;
			}
			return out;
		}

		inline std::ostream& operator<<(std::ostream& out, StrokeLineJoin line_join_) {
			using namespace std::literals;
			switch (static_cast<int>(line_join_)) {
			case 0:
				out << "arcs"sv;
				break;
			case 1:
				out << "bevel"sv;
				break;
			case 2:
				out << "miter"sv;
				break;
			case 3:
				out << "miter-clip"sv;
				break;
			case 4:
				out << "round"sv;
				break;
			}
			return out;
		}

		struct Point {
			Point() = default;
			Point(double x, double y)
				: x(x)
				, y(y) {
			}
			double x = 0.0;
			double y = 0.0;
		};


		/*
		 * Вспомогательная структура, хранящая контекст для вывода SVG-документа с отступами.
		 * Хранит ссылку на поток вывода, текущее значение и шаг отступа при выводе элемента
		 */
		struct RenderContext {
			RenderContext(std::ostream& out)
				: out(out) {
			}

			RenderContext(std::ostream& out, int indent_step, int indent = 0)
				: out(out)
				, indent_step(indent_step)
				, indent(indent) {
			}

			RenderContext Indented() const {
				return { out, indent_step, indent + indent_step };
			}

			void RenderIndent() const {
				for (int i = 0; i < indent; ++i) {
					out.put(' ');
				}
			}

			std::ostream& out;
			int indent_step = 0;
			int indent = 0;
		};


		/*
		 * Абстрактный базовый класс Object служит для унифицированного хранения
		 * конкретных тегов SVG-документа
		 * Реализует паттерн "Шаблонный метод" для вывода содержимого тега
		 */
		class Object {
		public:
			void Render(const RenderContext& context) const;

			virtual ~Object() = default;

		private:
			virtual void RenderObject(const RenderContext& context) const = 0;

		protected:
			Object() = default;
		};


		template <typename Owner>
		class PathProps {
		public:
			Owner& SetFillColor(Color color) {
				fill_color_ = std::move(color);
				return AsOwner();
			}

			Owner& SetStrokeColor(Color color) {
				stroke_color_ = std::move(color);
				return AsOwner();
			}

			Owner& SetStrokeWidth(double width) {
				stroke_width_ = std::move(width);
				return AsOwner();
			}

			Owner& SetStrokeLineCap(StrokeLineCap line_cap) {
				line_cap_ = std::move(line_cap);
				return AsOwner();
			}

			Owner& SetStrokeLineJoin(StrokeLineJoin line_join) {
				line_join_ = std::move(line_join);
				return AsOwner();
			}

		protected:
			~PathProps() = default;

			void RenderAttrs(std::ostream& out) const {
				using namespace std::literals;

				if (fill_color_) {
					out << " fill=\""sv << *fill_color_ << "\""sv;
				}
				if (stroke_color_) {
					out << " stroke=\""sv << *stroke_color_ << "\""sv;
				}
				if (stroke_width_) {
					out << " stroke-width=\""sv << *stroke_width_ << "\""sv;
				}
				if (line_cap_) {
					out << " stroke-linecap=\""sv << *line_cap_ << "\""sv;
				}
				if (line_join_) {
					out << " stroke-linejoin=\""sv << *line_join_ << "\""sv;
				}
			}

		private:
			Owner& AsOwner() {
			
				return static_cast<Owner&>(*this);
			}

			std::optional<Color> fill_color_;
			std::optional<Color> stroke_color_;
			std::optional<double> stroke_width_;
			std::optional<StrokeLineCap> line_cap_;
			std::optional<StrokeLineJoin> line_join_;
		};


		/*
		 * Класс Circle моделирует элемент <circle> для отображения круга
	
		 */
		class Circle final : public Object, public PathProps<Circle> {
		public:
			Circle& SetCenter(Point center);
			Circle& SetRadius(double radius);

		private:
			void RenderObject(const RenderContext& context) const override;

			Point center_;
			double radius_ = 1.0;
		};


		/*
		 * Класс Polyline моделирует элемент <polyline> для отображения ломаных линий
	 
		 */
		class Polyline final : public Object, public PathProps<Polyline> {
		public:
			// Добавляет очередную вершину к ломаной линии
			Polyline& AddPoint(Point point);

		private:
			std::vector<Point> points_;

			void RenderObject(const RenderContext& context) const override;
		};


		/*
		 * Класс Text моделирует элемент <text> для отображения текста
		 * https://developer.mozilla.org/en-US/docs/Web/SVG/Element/text
		 */
		class Text final : public Object, public PathProps<Text> {
		public:
			// Задаёт координаты опорной точки (атрибуты x и y)
			Text& SetPosition(Point pos);

			// Задаёт смещение относительно опорной точки (атрибуты dx, dy)
			Text& SetOffset(Point offset);

			// Задаёт размеры шрифта (атрибут font-size)
			Text& SetFontSize(uint32_t size);

			// Задаёт название шрифта (атрибут font-family)
			Text& SetFontFamily(const std::string& font_family);

			// Задаёт толщину шрифта (атрибут font-weight)
			Text& SetFontWeight(const std::string& font_weight);

			// Задаёт текстовое содержимое объекта (отображается внутри тега text)
			Text& SetData(const std::string& data);

		private:
			Point pos_ = { 0, 0 };
			Point offset_ = { 0, 0 };
			uint32_t font_size_ = 1;
			std::string font_family_;
			std::string font_weight_;
			std::string data_ = "";

			void PrintText(const RenderContext& context) const;

			void RenderObject(const RenderContext& context) const override;
		};


		class ObjectContainer {
		public:

			virtual ~ObjectContainer() = default;
		

			virtual void AddPtr(std::unique_ptr<Object>&& obj) = 0;

		

		
			
			//~ObjectContainer() = default;
		};


		class Drawable {
		public:
			virtual void Draw(ObjectContainer& container) const = 0;

			virtual ~Drawable() = default;
		};


		class Document : public ObjectContainer {
		public:
			// Метод Add добавляет в svg-документ любой объект-наследник svg::Object.
			// Пример использования:
			// Document doc;
			// doc.Add(Circle().SetCenter({20, 30}).SetRadius(15));

			Document() = default;

			template <typename Obj>
			void Add(Obj obj) {
				AddPtr(std::make_unique<Obj>(std::move(obj)));
			}
		
			// Добавляет в svg-документ объект-наследник svg::Object
			void AddPtr(std::unique_ptr<Object>&& obj) override;

			// Выводит в ostream svg-представление документа
			void Render(std::ostream& out) const;

			size_t Size();

			void Clear();
		private:
			std::vector<std::shared_ptr<Object>> items_;
		};

	}  // namespace svg
	
