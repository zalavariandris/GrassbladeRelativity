#pragma once
#include "glm/glm.hpp"
#include "Curve.h"
#include "Segment.h"
#include <vector>
#include <memory> // enable shared from this

class Path : public std::enable_shared_from_this<Path>
{
	std::vector<Segment> _segments;
	std::vector<Curve> _curves;
	int Path::_countCurves();
	bool _closed;
public:

	Path();
	Path(std::vector<Segment> segments);

	void add(Segment segment);
	void add(glm::vec2 point);
	void draw();

	std::vector<Curve> getCurves();
	CurveLocation getLocationAt(double offset);
};

