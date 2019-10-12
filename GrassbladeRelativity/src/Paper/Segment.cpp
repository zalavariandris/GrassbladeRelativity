#pragma once
#include "Segment.h"
#include "Path.h"
#include <iostream>
namespace Paper {
	Segment::Segment() :
		_point(glm::vec2(0)), _handleIn(glm::vec2(0)), _handleOut(glm::vec2(0)) {};

	Segment::Segment(glm::vec2 point) :
		_point(point), _handleIn(glm::vec2(0)), _handleOut(glm::vec2(0)) {};

	Segment::Segment(glm::vec2 point, glm::vec2 handleIn, glm::vec2 handleOut) :
		_point(point), _handleIn(handleIn), _handleOut(handleOut) {};

	// shoud be private, but friends with curve and path
	glm::vec2 Segment::point() const {
		return _point;
	}

	void Segment::point(glm::vec2 val) {
		if (_path != nullptr) {
			_path->LengthNeedsUpdate = true;
		}
		_point = val;
	}

	glm::vec2 Segment::handleIn() const {
		return _handleIn;
	}
	void Segment::handleIn(glm::vec2 val) {
		if (_path!=nullptr) {
			_path->LengthNeedsUpdate = true;
		}
		_handleIn = val;
	}

	glm::vec2 Segment::handleOut() const {
		return _handleOut;
	}
	void Segment::handleOut(glm::vec2 val) {
		if (_path!=nullptr) {
			_path->LengthNeedsUpdate = true;
		}
		_handleOut = val;
	}
}


