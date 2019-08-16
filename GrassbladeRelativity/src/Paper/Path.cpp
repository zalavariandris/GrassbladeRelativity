#include "Path.h"
#include "CurveLocation.h"

Path::Path() {
	_closed = false;
}

Path::Path(std::vector<Segment> segments) {
	_closed = false;
}

void Path::add(Segment segment) {
	_segments.push_back(segment);
}

void Path::add(glm::vec2 point) {
	add(Segment(point));
}

void Path::draw() {
	//for (Curve curve : getCurves()) {
	//	curve.draw();
	//}
}

//CurveLocation Path::getLocationOf(glm::vec2 point) {
//	auto curves = getCurves();
//	for (auto i = 0; i < curves.size(); i++) {
//		CurveLocation loc = curves[i].getLocationOf(point);
//		if (loc)
//			return loc;
//	}
//	return CurveLocation();
//};

CurveLocation Path::getLocationAt(double offset) {
	auto curves = getCurves();
	int length = 0;
	for (auto i = 0; i < curves.size(); i++) {
		auto start = length;
		Curve curve = curves[i];
		length += curve.getLength();
		if (length > offset) {
			// Found the segment within which the length lies
			return curve.getLocationAt(offset - start);
		}
	}

	// TODO: !!!!
	//// It may be that through imprecision of getLength, that the end of
	//// the last curve was missed:
	//if (curves.size() > 0 && offset <= getLength()) {
	//	return CurveLocation(curves[curves.size() - 1], 1);
	//}
	return CurveLocation(); //return invalid curve location
}

int Path::_countCurves() {
	auto length = _segments.size();
	// Reduce length by one if it's an open path:
	return !_closed && length > 0 ? length - 1 : length;
}

std::vector<Curve> Path::getCurves() {
	bool CurvesNeedsUpdate = _curves.size() > 0 ? false : true;
	if (CurvesNeedsUpdate) {
		auto length = _countCurves();
		_curves = std::vector<Curve>(length);
		for (auto i = 0; i < length; i++) {
			_curves[i] = Curve(weak_from_this(), _segments[i],
				// Use first segment for segment2 of closing curve
				length > _segments.size()-1 ? _segments[i+1] : _segments[0]);
		}
	}
	return _curves;
}