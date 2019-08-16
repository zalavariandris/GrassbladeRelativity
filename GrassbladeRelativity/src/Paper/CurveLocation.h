#pragma once
#include <memory>
#include "glm/glm.hpp"
#include <memory>
#include "Curve.h"

class CurveLocation {
public:
	double _time;
	Curve _curve;
	glm::vec2 _point;

	CurveLocation();

	CurveLocation(Curve curve, double time);

	bool isValid() const {
		return isnan(_time);
	}

	operator bool() const {
		return isValid();
	}
};
