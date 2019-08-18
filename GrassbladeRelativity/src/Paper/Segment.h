#pragma once

#include "glm/glm.hpp"
#include <memory>
class Curve;
class Path;
class Segment
{
public:
	Segment() :
		_point(glm::vec2(0)) {};

	Segment(glm::vec2 point)
		: _point(point) {};

	Segment(glm::vec2 point, glm::vec2 handleIn, glm::vec2 handleOut)
		: _point(point), _handleIn(handleIn), _handleOut(handleOut) {};

	// shoud be private, but friends with curve and path
	glm::vec2 _point;
	glm::vec2 _handleIn;
	glm::vec2 _handleOut;

	std::weak_ptr<Curve> _curve;
	std::weak_ptr<Path> _path;
};

