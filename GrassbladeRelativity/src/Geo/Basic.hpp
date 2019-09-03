#pragma once
#include "glm/glm.hpp"

namespace Geo {
	struct Point {
		double x;
		double y;
	};

	class Rectangle {
	private:
		double _x;
		double _y;
		double _w;
		double _h;
	public:
		Rectangle() {};
		Rectangle(glm::vec2 point, double width, double height) :
			_x(point.x), _y(point.y), _w(width), _h(height) {

		};

		Rectangle(glm::vec2 from, glm::vec2 to) {
			_x = from.x;
			_y = from.y;
			_w = to.x - from.x;
			_h = to.y - from.y;
			if (_w < 0) {
				_x = to.x;
				_w = -_w;
			}
			if (_h < 0) {
				_y = to.y;
				_h = -_h;
			}
		};

		double left() {
			return _x;
		}

		double right() {
			return _x + _w;
		}
		double top() {
			return _y;
		}

		double bottom() {
			return _y + _h;
		}

		bool contains(glm::vec2 point) {
			return point.x > left()
				&& point.x < right()
				&& point.y > top()
				&& point.y < bottom();
		}
	};

	struct Matrix {

	};
}