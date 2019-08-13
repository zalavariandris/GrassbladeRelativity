#pragma once
#include "Curve.h"
#include "Segment.h"
#include <vector>

class Path
{
	std::vector<Segment> segments;
public:
	Path();
	~Path();

	void add(Segment segment);
	void add(glm::vec2 point);
};

