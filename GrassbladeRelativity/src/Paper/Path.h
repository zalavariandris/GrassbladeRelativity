#pragma once
#include "glm/glm.hpp"
#include "Curve.h"
#include "Segment.h"
#include <vector>
#include <memory> // enable shared from this

class Path : public std::enable_shared_from_this<Path>
{

	int Path::_countCurves();
	bool _closed;
	int _version;
	bool CurvesNeedsUpdate{ true };
	bool LengthNeedsUpdate{ true };
	double _length;
public:
	std::vector<std::shared_ptr<Segment>> _segments;
	std::vector<std::shared_ptr<Curve>> _curves;
	Path();
	Path(std::vector<std::shared_ptr<Segment>> segments);

	std::vector<std::shared_ptr<Curve>> getCurves();
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

	std::shared_ptr<Curve> getFirstCurve();
	std::shared_ptr<Curve> getLastCurve();

	void draw();
	void add(std::shared_ptr<Segment> segment);
	void add(std::vector<std::shared_ptr<Segment>> segments);

	void insert(int index, std::shared_ptr<Segment> segment);
	void insert(int index, std::vector<std::shared_ptr<Segment>> segments);

	void _add(std::vector<std::shared_ptr<Segment>> segs, int index = -1);
	void _adjustCurves(int start, int end);

	std::shared_ptr<Segment> getFirstSegment();
	std::shared_ptr<Segment> getLastSegment();
};

