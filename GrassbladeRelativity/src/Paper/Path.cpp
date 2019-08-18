#include "Path.h"
#include "CurveLocation.h"
#include <iostream>
#include "numerical.h"
#include <algorithm> // for 'min' and 'max'
Path::Path(std::vector<Segment> segments) {
	_segments = segments;
	_closed = false;
	CurvesNeedsUpdate = true;
	LengthNeedsUpdate = true;
}


void Path::draw() {
	for (Curve curve : getCurves()) {
		curve.draw();
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
		Curve curve = curves[i];
		length += curve.getLength();
		if (length > offset) {
			// Found the segment within which the length lies
			return curve.getLocationAt(offset - start);
		}
	}

	// It may be that through imprecision of getLength, that the end of
	// the last curve was missed:
	if (curves.size() > 0 && offset <= getLength()) {
		return CurveLocation(curves[curves.size() - 1], 1.0);
	}

	//return invalid curve location
	return CurveLocation(); 
}

Curve Path::getFirstCurve() {
	return getCurves()[0];
}
Curve Path::getLastCurve(){
	auto curves = getCurves();
	return curves[curves.size() - 1];
}

double Path::getNearestTime(glm::vec2 point) {
	auto curves = getCurves();
	double minDist = std::numeric_limits<double>::infinity();
	double minT;
	for (auto i = 0; i < curves.size(); i++) {
		double t = curves[i].getNearestTime(point);
		glm::vec2 P = curves[i].getPointAtTime(t);
		double distance = glm::distance(point, P);
		if (distance < minDist) {
			minT = t;
			minDist = distance;
		}
	}
	return minDist;
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
		return CurveLocation(getFirstCurve(), 0.0);

	if (t > maxT)
		return CurveLocation(getLastCurve(), 1.0);

	//
	int i = t * curves.size();

	double curvesTime = t*curves.size()-i;
	return curves[i].getLocationAtTime(curvesTime);
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
		return loc._curve.getNormal(loc._time);
	return glm::vec2(NAN);
}

glm::vec2 Path::getTangentAtTime(double t) {
	CurveLocation loc = getLocationAtTime(t);
	if (loc)
		return loc._curve.getTangent(loc._time);
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
		return loc._curve.getTangent(loc._time);
	return glm::vec2(NAN);
}

glm::vec2 Path::getNormalAt(double offset) {
	CurveLocation loc = getLocationAt(offset);
	return loc._curve.getNormal(loc._time);
}

int Path::_countCurves() {
	auto length = _segments.size();
	// Reduce length by one if it's an open path:
	return !_closed && length > 0 ? length - 1 : length;
}

std::vector<Curve> Path::getCurves() {
	if (CurvesNeedsUpdate) {
		auto length = _countCurves();
		_curves = std::vector<Curve>(length);
		for (auto i = 0; i < length; i++) {
			_curves[i] = Curve(weak_from_this(), _segments[i], _segments[i + 1]);
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
			length += curves[i].getLength();
		}
		_length = length;
		LengthNeedsUpdate = false;
	}
	return _length;
}
