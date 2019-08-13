#pragma once

#include "glm/glm.hpp"

class Segment
{
public:
	Segment(glm::vec2 point)
		: point(point) {};

	Segment(glm::vec2 point, glm::vec2 handleIn, glm::vec2 handleOut)
		: point(point), handleIn(handleIn), handleOut(handleOut) {};

private:
	glm::vec2 point;
	glm::vec2 handleIn;
	glm::vec2 handleOut;
};

