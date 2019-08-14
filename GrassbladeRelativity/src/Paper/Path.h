#pragma once
#include "glm/glm.hpp"
#include "Curve.h"
#include "Segment.h"
#include <vector>

class Path
{
	std::vector<Segment> segments;
	std::vector<Curve> curves;
public:


	void add(Segment segment);
	void add(glm::vec2 point);
	void draw();

	CurveLocation getLocationOf(glm::vec2 point);
};

