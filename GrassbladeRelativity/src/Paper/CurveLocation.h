#pragma once
#include <memory>
#include "glm/glm.hpp"
#include <memory>
#include "Curve.h"
namespace Paper {
	class CurveLocation {
	public:
		double _time;
		Curve _curve;
		glm::vec2 _point;
		glm::vec2 _tangent;
		glm::vec2 _normal;

		CurveLocation();

		CurveLocation(Curve curve, double time);

		glm::vec2 point() {
			return _point;
		}

		glm::vec2 tangent() {
			return _tangent;
		}

		glm::vec2 normal() {
			return _normal;
		}

		bool isValid() const {
			return !isnan(_time);
		}

		operator bool() const {
			return isValid();
		}
	};
}
