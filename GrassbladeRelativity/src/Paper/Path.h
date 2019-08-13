#pragma once
#include "glm/glm.hpp"
#include "Curve.h"
#include "Segment.h"
#include <vector>

class Path
{
	std::vector<Segment> segments;
public:


	void add(Segment segment);
	void add(glm::vec2 point);
	void draw();
};

