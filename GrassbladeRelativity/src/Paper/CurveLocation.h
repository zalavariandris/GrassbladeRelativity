#pragma once
#include <memory>
#include "glm/glm.hpp"
#include <memory>
class Curve;
class CurveLocation {
public:
	double _time;
	std::shared_ptr<Curve> _curve;
	glm::vec2 _point;

	CurveLocation();

	CurveLocation(std::shared_ptr<Curve> curve, double time);

	bool isValid() const {
		return isnan(_time);
	}

	operator bool() const {
		return isValid();
	}
};
