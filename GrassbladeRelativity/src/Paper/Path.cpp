#include "Path.h"

#include <algorithm> // for 'min' and 'max'
#include "numerical.h"
#include "CurveLocation.h"

Path::Path() {
	_closed = false;
}

Path::Path(std::vector<std::shared_ptr<Segment>> segments) {
	_add(segments);
	_closed = false;
}

void Path::draw() {
	for (auto curve : getCurves()) {
		curve->draw();
	}
}

/*
	Returns the curve location of the specified offset on the path.
	TODO: handle negative values.
*/
CurveLocation Path::getLocationAt(double offset) {
	if (offset < 0)
		return CurveLocation();

	auto curves = getCurves();
	double length = 0;
	for (auto i = 0; i < curves.size(); i++) {
		auto start = length;
		std::shared_ptr<Curve> curve = curves[i];
		length += curve->getLength();
		if (length > offset) {
			// Found the segment within which the length lies
			return curve->getLocationAt(offset - start);
		}
	}

	// It may be that through imprecision of getLength, that the end of
	// the last curve was missed:
	if (curves.size() > 0 && offset <= getLength()) {
		return CurveLocation(*curves[curves.size() - 1], 1.0);
	}

	//return invalid curve location
	return CurveLocation(); 
}

std::shared_ptr<Curve> Path::getFirstCurve() {
	return getCurves()[0];
}
std::shared_ptr<Curve> Path::getLastCurve(){
	auto curves = getCurves();
	return curves[curves.size() - 1];
}

double Path::getNearestTime(glm::vec2 point) {
	auto curves = getCurves();
	double minDist = std::numeric_limits<double>::infinity();
	double minT;
	int index;
	for (auto i = 0; i < curves.size(); i++) {
		double t = curves[i]->getNearestTime(point);
		glm::vec2 P = curves[i]->getPointAtTime(t);
		double distance = glm::distance(point, P);
		if (distance < minDist) {
			minT = t;
			minDist = distance;
			index = i;
		}
	}
	return (minT+index)/curves.size();
};

CurveLocation Path::getLocationAtTime(double t) {
	//
	if (t < 0.0 || t>1.0 || isnan(t))
		return CurveLocation();

	//
	auto curves = getCurves();

	//
	auto minT = Numerical::CURVETIME_EPSILON;
	auto maxT = 1 - minT;
	if (t < minT) // TODO: !!! probably using Numerical::CURVETIME_EPSILON is safer
		return CurveLocation(*getFirstCurve(), 0.0);

	if (t > maxT)
		return CurveLocation(*getLastCurve(), 1.0);

	//
	int i = t * curves.size();

	double curveTime = t*curves.size()-i;
	return curves[i]->getLocationAtTime(curveTime);
}

glm::vec2 Path::getPointAtTime(double t) {
	CurveLocation loc = getLocationAtTime(t);
	if(loc)
		return loc._point;
	return glm::vec2(NAN);
}

glm::vec2 Path::getNormalAtTime(double t) {
	CurveLocation loc = getLocationAtTime(t);
	if(loc)
		return loc._curve.getNormalAtTime(loc._time);
	return glm::vec2(NAN);
}

glm::vec2 Path::getTangentAtTime(double t) {
	CurveLocation loc = getLocationAtTime(t);
	if (loc)
		return loc._curve.getTangentAtTime(loc._time);
	return glm::vec2(NAN);
}

glm::vec2 Path::getPointAt(double offset) {
	CurveLocation loc = getLocationAt(offset);
	if(loc)
		return loc._point;
	return glm::vec2(NAN);
}

glm::vec2 Path::getTangentAt(double offset) {
	CurveLocation loc = getLocationAt(offset);
	if(loc)
		return loc._curve.getTangentAtTime(loc._time);
	return glm::vec2(NAN);
}

glm::vec2 Path::getNormalAt(double offset) {
	CurveLocation loc = getLocationAt(offset);
	return loc._curve.getNormalAtTime(loc._time);
}

int Path::_countCurves() {
	auto length = _segments.size();
	// Reduce length by one if it's an open path:
	return !_closed && length > 0 ? length - 1 : length;
}

std::vector<std::shared_ptr<Curve>> Path::getCurves() {
	if (CurvesNeedsUpdate) {
		auto length = _countCurves();
		_curves = std::vector<std::shared_ptr<Curve>>(length);
		for (auto i = 0; i < length; i++) {
			_curves[i] = std::make_shared<Curve>(/* TODO weak reference to path weak_from_this(),*/ _segments[i], _segments[i + 1]);
				//TODO:: closed path
				// Use first segment for segment2 of closing curve
				//i + 1 < _segments.size() ? _segments[i+1] : _segments[0]);
		}
		CurvesNeedsUpdate = false;
	}
	return _curves;
}

double Path::getLength() {
	if (LengthNeedsUpdate) {
		auto curves = getCurves();
		double length = 0;
		for (auto i = 0; i < curves.size(); i++) {
			length += curves[i]->getLength();
		}
		_length = length;
		LengthNeedsUpdate = false;
	}
	return _length;
}

void Path::add(std::shared_ptr<Segment> segment) {
	_add({ segment });
}

void Path::add(std::vector<std::shared_ptr<Segment>> segments) {
	_add(segments);
}

void Path::insert(int index, std::shared_ptr<Segment> segment) {
	_add({ segment }, index);
}
void Path::insert(int index, std::vector<std::shared_ptr<Segment>> segments) {
	_add(segments, index);
}

void Path::_add(std::vector<std::shared_ptr<Segment>> segs, int index) {
	// Local short-cuts:
	int amount = segs.size();
	bool append = index < 0;
	index = append ? _segments.size() : index;

	// Scan through segments to add first, convert if necessary and set
	// _path and _index references on them.
	for (auto i = 0; i < amount; i++) {
		auto segment = segs[i];
		// If the segments belong to another path already, clone them before
		// adding:
		// TODO: weak reference to path
		//if (segment->_path.lock())
		//	segment = segs[i] = std::make_shared<Segment>(*segment);// segment.clone();
		//segment->_path = weak_from_this();
		segment->_index = index + i;

	}
	if (append) {
		_segments.reserve(_segments.size() + segs.size());
		_segments.insert(_segments.end(), segs.begin(), segs.end());
	}
	else {
		_segments.reserve(_segments.size() + segs.size());
		_segments.insert(_segments.begin() + index, segs.begin(), segs.end());
	}
	// Keep the curves list in sync all the time in case it was requested
	// already.
	if (!CurvesNeedsUpdate) {
		auto total = _countCurves();
		// If we're adding a new segment to the end of an open path,
		// we need to step one index down to get its curve.
		auto start = index > 0 && index + amount - 1 == total ? index - 1 : index;
		auto insert = start;
		auto end = std::min(start + amount, total);

		// Insert new curves, but do not initialize their segments yet,
		// since #_adjustCurves() handles all that for us
		for (auto i = insert; i < end; i++) {
			_curves.insert(_curves.begin()+i, std::make_shared<Curve>(
				//weak_from_this(), 
				std::make_shared<Segment>(), 
				std::make_shared<Segment>()
				)
			);
		}
		// Adjust segments for the curves before and after the removed ones
		_adjustCurves(start, end);
	}
}

/**
* Adjusts segments of curves before and after inserted / removed segments.
*/
void Path::_adjustCurves(int start, int end) {
	std::shared_ptr<Curve> curve;
	for (auto i = start; i < end; i++) {
		curve = _curves[i];
		//curve->_path = weak_from_this();
		curve->_segment1 = _segments[i];
		curve->_segment2 = _segments[i + 1]; //TODO: closed path   || _segments[0];
		curve->_changed();
	}
	// If it's the first segment, correct the last segment of closed
	// paths too:
	int _ = _closed && !start ? _segments.size() - 1 : start - 1;
	if (_ < _curves.size() && _ >=0) {
		curve = _curves[_];
		curve->_segment2 = _segments[start >= 0 < _segments.size() ? start : 0];
		curve->_changed();
	}
	// Fix the segment after the modified range, if it exists
	if (end < _curves.size()) {
		curve = _curves[end];
		curve->_segment1 = _segments[end];
		curve->_changed();
	}
};

std::shared_ptr<Segment> Path::getFirstSegment() {
	return _segments[0];
}

std::shared_ptr<Segment> Path::getLastSegment() {
	return _segments[_segments.size() - 1];
}
