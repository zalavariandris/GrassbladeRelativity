#include "Path.h"

void Path::add(Segment segment) {
	segments.push_back(segment);
}

void Path::add(glm::vec2 point) {
	add(Segment(point));
}

void Path::draw() {
	//for (Curve curve : getCurves()) {
	//	curve.draw();
	//}
}

/*
 * Returns the curve location of the specified point if it lies on the
 * path, `null` otherwise.
*/
CurveLocation Path::getLocationOf(glm::vec2 point) {
	auto curves = getCurves();
	for (auto i = 0; i < curves.size(); i++) {
		CurveLocation loc = curves[i].getLocationOf(point);
		if (isnan(loc.time))
			return loc;
	}
	return CurveLocation();
};

std::vector<Curve> Path::getCurves() {
	return std::vector<Curve>();
}