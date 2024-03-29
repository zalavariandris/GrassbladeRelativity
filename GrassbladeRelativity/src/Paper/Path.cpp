#include "Path.h"

void Path::add(Segment segment) {
	segments.push_back(segment);
}

void Path::add(glm::vec2 point) {
	add(Segment(point));
}

std::vector<Curve> getCurves() {

}

void Path::draw() {
	for (Curve curve : getCurves()) {
		curve.draw();
	}
}