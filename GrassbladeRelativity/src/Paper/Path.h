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
	int _version;
	bool CurvesNeedsUpdate;
	bool LengthNeedsUpdate;
	double _length;
public:
	Path(std::vector<Segment> segments);

	std::vector<Curve> getCurves();
	CurveLocation getLocationAt(double offset);
	glm::vec2 getPointAt(double offset);
	glm::vec2 getTangentAt(double offset);
	glm::vec2 getNormalAt(double offset);

	CurveLocation getLocationAtTime(double t);
	glm::vec2 getPointAtTime(double t);
	glm::vec2 getTangentAtTime(double t);
	glm::vec2 getNormalAtTime(double t);

	double Path::getNearestTime(glm::vec2 point);

	double getLength();

	Curve getFirstCurve();
	Curve getLastCurve();

	void draw();
};

