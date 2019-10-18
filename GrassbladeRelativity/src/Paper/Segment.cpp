#pragma once
#include "Segment.h"
#include "Path.h"
#include <iostream>
namespace Paper {
	Segment::Segment() :
		mPoint(glm::vec2(0)),
		mHandleIn(glm::vec2(0)),
		mHandleOut(glm::vec2(0)) {};

	Segment::Segment(glm::vec2 point) :
		mPoint(point), 
		mHandleIn(glm::vec2(0)), 
		mHandleOut(glm::vec2(0)) {};

	Segment::Segment(glm::vec2 point, glm::vec2 handle) :
		mPoint(point),
		mHandleIn(-handle),
		mHandleOut(handle) {};

	Segment::Segment(glm::vec2 point, glm::vec2 handleIn, glm::vec2 handleOut) :
		mPoint(point),
		mHandleIn(handleIn),
		mHandleOut(handleOut) {};

	glm::vec2 Segment::point() const {
		return mPoint;
	}

	void Segment::point(glm::vec2 val) {
		assert(mPath != nullptr); // segment must be element of a path
		mPath->LengthNeedsUpdate = true;
		mPoint = val;
	}

	glm::vec2 Segment::handleIn() const {
		return mHandleIn;
	}

	void Segment::handleIn(glm::vec2 val) {
		assert(mPath != nullptr); // segment must be element of a path
		mPath->LengthNeedsUpdate = true;
		mHandleIn = val;
	}

	glm::vec2 Segment::handleOut() const {
		return mHandleOut;
	}

	void Segment::handleOut(glm::vec2 val) {
		assert(mPath != nullptr); // segment must be element of a path
		mPath->LengthNeedsUpdate = true;
		mHandleOut = val;
	}
}


